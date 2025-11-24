// Producer–Consumer with statistics (Problem 1) + batch mode + sample output file.
// Build: gcc -O2 -Wall -Wextra -pthread src/PRODCONS.c -o bin/PRODCONS -pthread
// Single run:   ./bin/PRODCONS <producers> <consumers> <buffer> [--fast]
// All 18 runs:  ./bin/PRODCONS --all [--fast] [--outfile sample_output.txt]
//
// Notes:
// - Default behavior follows spec: producers sleep 5–40 ms per item.
// - Use --fast (or env FAST_MODE=1) to disable sleeps for quick testing.
// - --all writes a complete sample output file (default: sample_output.txt).

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define TARGET_ITEMS        1000
#define PRODUCE_MIN_US      5000      // 5 ms (spec)
#define PRODUCE_MAX_US      40000     // 40 ms (spec)

typedef struct {
    uint8_t day;   // 1–30
    uint8_t month; // 1–12
    uint8_t year;  // 16
    int     store; // [1..P]
    int     reg;   // [1..6]
    double  amount; // 0.50..999.99
} Sale;

typedef struct {
    // Circular buffer
    Sale *buf;
    int capacity;
    int head, tail;

    // Sync
    sem_t empty, full;
    pthread_mutex_t qmtx;

    // Counters/state
    atomic_int produced_total;
    atomic_int consumed_total;
    atomic_int done;
    int P, C, B;

    // Output for this run
    FILE *out;

    // Fast mode?
    int fast_mode;
} Shared;

typedef struct {
    double *store_totals;     // size P
    double  month_totals[12]; // Jan..Dec
    double  aggregate;
    pthread_mutex_t mtx;
    FILE *out;
} GlobalStats;

typedef struct {
    int cid, P;
    double *store_totals;     // size P
    double  month_totals[12];
    double  aggregate;
} LocalStats;

// -------------------- Globals (reset per run) --------------------
static Shared G;
static GlobalStats GSTATS;

// -------------------- Utils --------------------
static inline int rand_range(int lo, int hi){ return lo + rand() % (hi - lo + 1); }
static inline double rand_amount(void){ return (double)rand_range(50, 99999) / 100.0; }

// -------------------- Producer --------------------
static void *producer(void *arg){
    int id = (int)(intptr_t)arg;          // 0..P-1
    unsigned seed = (unsigned)time(NULL) ^ (0x9e3779b9u * (unsigned)(id + 1) ^ (unsigned)pthread_self());
    srand(seed);

    for(;;){
        if (atomic_load(&G.produced_total) >= TARGET_ITEMS) break;

        Sale s;
        s.day    = (uint8_t)rand_range(1, 30);
        s.month  = (uint8_t)rand_range(1, 12);
        s.year   = 16;
        s.store  = id + 1;               // stable mapping: producer -> store
        s.reg    = rand_range(1, 6);
        s.amount = rand_amount();

        sem_wait(&G.empty);
        pthread_mutex_lock(&G.qmtx);

        if (atomic_load(&G.produced_total) >= TARGET_ITEMS){
            pthread_mutex_unlock(&G.qmtx);
            sem_post(&G.full);           // nudge a consumer
            break;
        }

        G.buf[G.tail] = s;
        G.tail = (G.tail + 1) % G.capacity;
        atomic_fetch_add(&G.produced_total, 1);

        pthread_mutex_unlock(&G.qmtx);
        sem_post(&G.full);

        if (!G.fast_mode){
            int delay = rand_range(PRODUCE_MIN_US, PRODUCE_MAX_US);
            usleep((useconds_t)delay);
        }
    }
    return NULL;
}

// -------------------- Consumer --------------------
static void *consumer(void *arg){
    LocalStats *L = (LocalStats*)arg;

    for(;;){
        sem_wait(&G.full);
        pthread_mutex_lock(&G.qmtx);

        int produced = atomic_load(&G.produced_total);
        int consumed = atomic_load(&G.consumed_total);

        if (atomic_load(&G.done) && consumed >= produced){
            pthread_mutex_unlock(&G.qmtx);
            break;
        }

        if (consumed < produced){
            Sale s = G.buf[G.head];
            G.head = (G.head + 1) % G.capacity;
            atomic_fetch_add(&G.consumed_total, 1);

            pthread_mutex_unlock(&G.qmtx);
            sem_post(&G.empty);

            // Local stats (thread-local, no lock)
            if (s.store >= 1 && s.store <= L->P) L->store_totals[s.store - 1] += s.amount;
            if (s.month >= 1 && s.month <= 12)   L->month_totals[s.month - 1] += s.amount;
            L->aggregate += s.amount;
        }else{
            pthread_mutex_unlock(&G.qmtx);
        }
    }

    // Merge and print local summary
    pthread_mutex_lock(&GSTATS.mtx);

    for (int i = 0; i < L->P; ++i) GSTATS.store_totals[i] += L->store_totals[i];
    for (int m = 0; m < 12; ++m)  GSTATS.month_totals[m] += L->month_totals[m];
    GSTATS.aggregate += L->aggregate;

    if (G.out){
        fprintf(G.out, "\n--- Consumer %d summary ---\n", L->cid);
        fprintf(G.out, "Local aggregate: %.2f\n", L->aggregate);

        int top1=-1, top2=-1;
        for (int i=0;i<L->P;++i){
            if (top1==-1 || L->store_totals[i] > L->store_totals[top1]){ top2=top1; top1=i; }
            else if (top2==-1 || L->store_totals[i] > L->store_totals[top2]){ top2=i; }
        }
        if (top1!=-1) fprintf(G.out, "Top store: %d total=%.2f\n", top1+1, L->store_totals[top1]);
        if (top2!=-1) fprintf(G.out, "Next store: %d total=%.2f\n", top2+1, L->store_totals[top2]);
        fflush(G.out);
    }

    pthread_mutex_unlock(&GSTATS.mtx);
    return NULL;
}

// -------------------- Printing helpers --------------------
static void print_global_tables(FILE *f, const GlobalStats *S, int P){
    static const char *mname[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    fprintf(f, "\n==== Overall Per-Store Totals ====\n");
    for (int i=0;i<P;++i) fprintf(f, "Store %2d: %.2f\n", i+1, S->store_totals[i]);

    fprintf(f, "\n==== Overall Per-Month Totals ====\n");
    for (int m=0;m<12;++m) fprintf(f, "%s: %.2f\n", mname[m], S->month_totals[m]);

    fprintf(f, "\n==== Overall Aggregate ====\n");
    fprintf(f, "TOTAL: %.2f\n", S->aggregate);
    fflush(f);
}

// -------------------- One simulation run --------------------
static int run_simulation(int P, int C, int B, FILE *out, int fast_mode){
    // Reset global state
    memset(&G, 0, sizeof(G));
    memset(&GSTATS, 0, sizeof(GSTATS));
    G.P=P; G.C=C; G.B=B; G.out=out; G.fast_mode=fast_mode;

    // Seed once per run for variety
    srand((unsigned)time(NULL) ^ (unsigned)(P*100 + C*10 + B));

    // Buffer & sync
    G.capacity = G.B;
    G.buf = (Sale*)calloc((size_t)G.capacity, sizeof(Sale));
    if(!G.buf){ perror("calloc buffer"); return 1; }
    G.head = G.tail = 0;
    atomic_store(&G.produced_total, 0);
    atomic_store(&G.consumed_total, 0);
    atomic_store(&G.done, 0);

    if(sem_init(&G.empty,0,(unsigned)G.capacity)!=0){ perror("sem_init empty"); return 1; }
    if(sem_init(&G.full, 0,0)!=0){ perror("sem_init full"); return 1; }
    if(pthread_mutex_init(&G.qmtx,NULL)!=0){ perror("pthread_mutex_init qmtx"); return 1; }

    // Global stats
    GSTATS.store_totals = (double*)calloc((size_t)G.P, sizeof(double));
    if(!GSTATS.store_totals){ perror("calloc store_totals"); return 1; }
    if(pthread_mutex_init(&GSTATS.mtx,NULL)!=0){ perror("pthread_mutex_init stats"); return 1; }
    GSTATS.out = out;

    // Threads & locals
    pthread_t *pt = (pthread_t*)calloc((size_t)G.P, sizeof(pthread_t));
    pthread_t *ct = (pthread_t*)calloc((size_t)G.C, sizeof(pthread_t));
    if(!pt || !ct){ perror("calloc threads"); return 1; }

    LocalStats *locals = (LocalStats*)calloc((size_t)G.C, sizeof(LocalStats));
    if(!locals){ perror("calloc locals"); return 1; }
    for (int i=0;i<G.C;++i){
        locals[i].cid=i; locals[i].P=G.P;
        locals[i].store_totals = (double*)calloc((size_t)G.P, sizeof(double));
        if(!locals[i].store_totals){ perror("calloc local store_totals"); return 1; }
        memset(locals[i].month_totals, 0, sizeof(locals[i].month_totals));
        locals[i].aggregate=0.0;
    }

    // Timing
    struct timespec t0,t1;
    clock_gettime(CLOCK_MONOTONIC,&t0);

    // Create threads
    for(int i=0;i<G.P;++i)
        if(pthread_create(&pt[i],NULL,producer,(void*)(intptr_t)i)!=0){ perror("pthread_create producer"); return 1; }
    for(int i=0;i<G.C;++i)
        if(pthread_create(&ct[i],NULL,consumer,&locals[i])!=0){ perror("pthread_create consumer"); return 1; }

    // Finish producers, then mark done & wake consumers
    for(int i=0;i<G.P;++i) pthread_join(pt[i],NULL);
    atomic_store(&G.done,1);
    for(int i=0;i<G.C;++i) sem_post(&G.full); // ensure all waiting consumers wake

    // Finish consumers
    for(int i=0;i<G.C;++i) pthread_join(ct[i],NULL);

    // Timing
    clock_gettime(CLOCK_MONOTONIC,&t1);
    double elapsed_ms = (t1.tv_sec - t0.tv_sec)*1000.0 + (t1.tv_nsec - t0.tv_nsec)/1e6;

    // Output
    if(out){
        fprintf(out, "\n====================================\n");
        fprintf(out, "RUN  P=%d  C=%d  B=%d\n", P, C, B);
        fprintf(out, "Produced=%d  Consumed=%d  Time=%.2f ms\n",
                atomic_load(&G.produced_total),
                atomic_load(&G.consumed_total),
                elapsed_ms);
        print_global_tables(out, &GSTATS, G.P);
        fprintf(out, "====================================\n\n");
        fflush(out);
    }else{
        printf("\n====================================\n");
        printf("Produced=%d  Consumed=%d  Time=%.2f ms\n",
               atomic_load(&G.produced_total),
               atomic_load(&G.consumed_total),
               elapsed_ms);
        print_global_tables(stdout, &GSTATS, G.P);
    }

    // Cleanup
    for(int i=0;i<G.C;++i) free(locals[i].store_totals);
    free(locals);
    free(pt); free(ct);
    pthread_mutex_destroy(&GSTATS.mtx);
    free(GSTATS.store_totals);
    pthread_mutex_destroy(&G.qmtx);
    sem_destroy(&G.empty);
    sem_destroy(&G.full);
    free(G.buf);

    return 0;
}

// -------------------- Main --------------------
int main(int argc, char **argv){
    int fast_mode = 0;
    const char *outfile = "sample_output.txt";

    // Recognize env-based fast mode too
    const char *fm = getenv("FAST_MODE");
    if (fm && (strcmp(fm,"1")==0 || strcasecmp(fm,"true")==0)) fast_mode = 1;

    if (argc >= 2 && strcmp(argv[1],"--all")==0){
        // Parse optional flags
        for (int i=2;i<argc;++i){
            if (strcmp(argv[i],"--fast")==0) fast_mode = 1;
            else if (strcmp(argv[i],"--outfile")==0 && i+1<argc) { outfile = argv[i+1]; ++i; }
            else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                return 1;
            }
        }

        FILE *out = fopen(outfile, "w");
        if(!out){ perror("fopen sample_output"); return 1; }

        fprintf(out, "CS471/571 – Problem 1 (PRODCONS)\n");
        fprintf(out, "All 18 runs (p in {2,5,10}, c in {2,5,10}, b in {3,10})\n");
        fprintf(out, "Each run produces 1000 items; %s mode.\n\n", fast_mode ? "FAST (no sleeps)" : "SPEC (5–40ms sleeps)");
        fflush(out);

        const int Pset[] = {2,5,10};
        const int Cset[] = {2,5,10};
        const int Bset[] = {3,10};

        // Console progress
        printf("Starting 18 runs -> %s (%s)...\n", outfile, fast_mode ? "FAST" : "SPEC");
        fflush(stdout);

        for (int ip=0; ip<3; ++ip){
            for (int ic=0; ic<3; ++ic){
                for (int ib=0; ib<2; ++ib){
                    int P=Pset[ip], C=Cset[ic], B=Bset[ib];
                    fprintf(out, "---------- Starting run: P=%d  C=%d  B=%d ----------\n", P, C, B);
                    fflush(out);
                    printf("Run P=%d C=%d B=%d...\n", P, C, B);
                    fflush(stdout);

                    int rc = run_simulation(P,C,B,out,fast_mode);
                    if (rc != 0){
                        fprintf(out, "Run P=%d C=%d B=%d failed (rc=%d)\n\n", P,C,B,rc);
                        fflush(out);
                    }
                }
            }
        }

        fprintf(out, "\nAll runs complete.\n");
        fclose(out);
        printf("All 18 runs complete. Wrote: %s\n", outfile);
        return 0;
    }

    // Single-run mode
    if (!(argc==4 || (argc==5 && strcmp(argv[4],"--fast")==0))){
        fprintf(stderr,
            "Usage:\n"
            "  %s <producers> <consumers> <buffer> [--fast]\n"
            "  %s --all [--fast] [--outfile <path>]\n",
            argv[0], argv[0]);
        return 1;
    }

    int P = atoi(argv[1]);
    int C = atoi(argv[2]);
    int B = atoi(argv[3]);
    if (argc==5 && strcmp(argv[4],"--fast")==0) fast_mode = 1;

    if (P<=0 || C<=0 || B<=0){
        fprintf(stderr, "All arguments must be positive integers.\n");
        return 1;
    }

    // Single run -> stdout
    return run_simulation(P,C,B,/*out*/NULL,fast_mode);
}
