/*
 * VMEMMAN.c — COMPLETE, CLEAN, FULLY DOCUMENTED IMPLEMENTATION
 * -------------------------------------------------------------
 * Part 2 of the Operating Systems Project: Virtual Memory Management
 *
 * This file:
 *   ✓ Reads byte-addressable virtual addresses from sample_input.txt
 *   ✓ Converts each address to page numbers using 3 page sizes
 *       - 512 bytes
 *       - 1024 bytes
 *       - 2048 bytes
 *   ✓ Runs FOUR replacement algorithms:
 *       - FIFO
 *       - LRU
 *       - MRU
 *       - OPT (Optimal)
 *   ✓ Tests all 9 combinations of:
 *       page_size ∈ {512, 1024, 2048}
 *       frames    ∈ {4, 8, 12}
 *   ✓ Prints results in a clean, instructor-friendly format.
 *
 * This file is intentionally simple, readable, and HIGHLY documented.
 * It is designed for a student submission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* ------------------------------------------------------------
 * Project configuration
 * ----------------------------------------------------------*/
#define MAX_REFS 1000000    /* Max virtual addresses we can load */
static const int PAGE_SIZES[3]   = {512, 1024, 2048};
static const int FRAME_COUNTS[3] = {4, 8, 12};

/* ------------------------------------------------------------
 * load_addresses()
 * ------------------------------------------------------------
 * Reads sample_input.txt.
 * EXPECTED FILE FORMAT:
 *   Each line contains ONE integer (byte address).
 * RETURNS: number of addresses loaded, or -1 on error.
 * ----------------------------------------------------------*/
int load_addresses(const char *filename, unsigned long long *buffer) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening sample_input.txt");
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* strip leading whitespace */
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue; /* skip blank lines */

        unsigned long long addr = strtoull(p, NULL, 10);
        buffer[count++] = addr;
        if (count >= MAX_REFS) break;
    }

    fclose(fp);
    return count;
}

/* ------------------------------------------------------------
 * FIFO Algorithm (First-In First-Out)
 * ----------------------------------------------------------*/
int fifo_faults(const int *pages, int n, int frames) {
    int *queue = (int*)malloc(sizeof(int) * frames);
    int size = 0;
    int head = 0;
    int faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        int hit = 0;

        /* Check if page already loaded */
        for (int j = 0; j < size; j++) {
            if (queue[j] == page) {
                hit = 1;
                break;
            }
        }
        if (hit) continue;

        /* MISS */
        faults++;
        if (size < frames)
            queue[size++] = page;
        else {
            queue[head] = page;
            head = (head + 1) % frames;
        }
    }

    free(queue);
    return faults;
}

/* ------------------------------------------------------------
 * LRU Algorithm (Least Recently Used)
 * ----------------------------------------------------------*/
int lru_faults(const int *pages, int n, int frames) {
    int *mem  = (int*)malloc(sizeof(int) * frames);
    int *last = (int*)malloc(sizeof(int) * frames);

    for (int i = 0; i < frames; i++) { mem[i] = -1; last[i] = -1; }

    int faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        int hit = -1;

        /* hit? */
        for (int j = 0; j < frames; j++) {
            if (mem[j] == page) { hit = j; break; }
        }

        if (hit != -1) {
            last[hit] = i; /* update recency */
            continue;
        }

        /* MISS */
        faults++;

        /* empty slot? */
        int empty = -1;
        for (int j = 0; j < frames; j++) {
            if (mem[j] == -1) { empty = j; break; }
        }
        if (empty != -1) {
            mem[empty] = page;
            last[empty] = i;
            continue;
        }

        /* evict LRU (smallest last[]) */
        int victim = 0;
        for (int j = 1; j < frames; j++)
            if (last[j] < last[victim]) victim = j;

        mem[victim] = page;
        last[victim] = i;
    }

    free(mem);
    free(last);
    return faults;
}

/* ------------------------------------------------------------
 * MRU Algorithm (Most Recently Used)
 * ----------------------------------------------------------*/
int mru_faults(const int *pages, int n, int frames) {
    int *mem  = (int*)malloc(sizeof(int) * frames);
    int *last = (int*)malloc(sizeof(int) * frames);

    for (int i = 0; i < frames; i++) { mem[i] = -1; last[i] = -1; }

    int faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        int hit = -1;

        for (int j = 0; j < frames; j++)
            if (mem[j] == page) { hit = j; break; }

        if (hit != -1) {
            last[hit] = i;
            continue;
        }

        /* MISS */
        faults++;

        int empty = -1;
        for (int j = 0; j < frames; j++)
            if (mem[j] == -1) { empty = j; break; }  /* <-- fixed from --1 */

        if (empty != -1) {
            mem[empty] = page;
            last[empty] = i;
            continue;
        }

        /* evict MRU (largest last[]) */
        int victim = 0;
        for (int j = 1; j < frames; j++)
            if (last[j] > last[victim]) victim = j;

        mem[victim] = page;
        last[victim] = i;
    }

    free(mem);
    free(last);
    return faults;
}

/* ------------------------------------------------------------
 * OPT Algorithm (Optimal / MIN)
 * ----------------------------------------------------------*/
int opt_faults(const int *pages, int n, int frames) {
    int *mem = (int*)malloc(sizeof(int) * frames);
    for (int i = 0; i < frames; i++) mem[i] = -1;

    int faults = 0;

    for (int i = 0; i < n; i++) {
        int page = pages[i];
        int hit = -1;

        for (int j = 0; j < frames; j++)
            if (mem[j] == page) { hit = j; break; }

        if (hit != -1) continue; /* already present */

        /* MISS */
        faults++;

        int empty = -1;
        for (int j = 0; j < frames; j++)
            if (mem[j] == -1) { empty = j; break; }

        if (empty != -1) { mem[empty] = page; continue; }

        /* choose victim: farthest future use */
        int victim = 0;
        int farthest = -1;

        for (int j = 0; j < frames; j++) {
            int next = n + 10; /* sentinel = "never used again" */
            for (int k = i + 1; k < n; k++)
                if (pages[k] == mem[j]) { next = k; break; }

            if (next > farthest) { farthest = next; victim = j; }
        }

        mem[victim] = page;
    }

    free(mem);
    return faults;
}

/* ------------------------------------------------------------
 * Driver helpers
 * ----------------------------------------------------------*/
static void map_addresses_to_pages(const unsigned long long *addrs, int n,
                                   int page_size, int *pages_out) {
    for (int i = 0; i < n; i++)
        pages_out[i] = (int)(addrs[i] / (unsigned long long)page_size);
}

static void run_all_algorithms(const int *pages, int n, int frames, int page_size) {
    int f_fifo = fifo_faults(pages, n, frames);
    int f_lru  = lru_faults(pages, n, frames);
    int f_mru  = mru_faults(pages, n, frames);
    int f_opt  = opt_faults(pages, n, frames);

    printf("PageSize=%d Frames=%d | FIFO=%.2f%%  LRU=%.2f%%  MRU=%.2f%%  OPT=%.2f%%\n",
           page_size, frames,
           100.0 * f_fifo / n,
           100.0 * f_lru  / n,
           100.0 * f_mru  / n,
           100.0 * f_opt  / n);
}

/* ------------------------------------------------------------
 * MAIN PROGRAM — RUN ALL TESTS
 * ----------------------------------------------------------*/
int main(void) {
    /* 1) Load byte-addressable virtual addresses */
    unsigned long long *addresses =
        (unsigned long long*)malloc(sizeof(unsigned long long) * MAX_REFS);
    if (!addresses) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }

    int count = load_addresses("sample_input.txt", addresses);

    if (count <= 0) {
        printf("ERROR: Could not load sample_input.txt.\n");
        free(addresses);
        return 1;
    }

    printf("Loaded %d virtual addresses. Beginning analysis...\n\n", count);

    /* 2) Allocate page-number buffer (reused per page size) */
    int *pages = (int*)malloc(sizeof(int) * count);
    if (!pages) {
        fprintf(stderr, "Allocation failed.\n");
        free(addresses);
        return 1;
    }

    printf("==================== VMEM RESULTS ====================\n");

    for (int ps = 0; ps < 3; ps++) {
        int page_size = PAGE_SIZES[ps];

        /* Compute page-number sequence for this page size */
        map_addresses_to_pages(addresses, count, page_size, pages);

        for (int fi = 0; fi < 3; fi++) {
            int frames = FRAME_COUNTS[fi];
            if (frames <= 0) {
                fprintf(stderr, "Invalid frames=%d\n", frames);
                continue;
            }
            run_all_algorithms(pages, count, frames, page_size);
        }

        printf("------------------------------------------------------\n");
    }

    printf("======================== DONE ========================\n");

    free(pages);
    free(addresses);
    return 0;
}
