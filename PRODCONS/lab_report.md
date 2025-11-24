# CS 471 / 571 — Problem 1 Report
## Producer–Consumer Simulation with Statistics
**Author:** William Poston  
**Date:** November 23, 2025  

---

## 1. Introduction

This report presents the analysis of 18 experimental runs of the Producer–Consumer simulation developed for Problem 1. The program models a multi-threaded bounded-buffer system implemented using POSIX threads, semaphores, mutexes, and atomic counters.

Each experimental run terminates after 1000 total sales records are produced. Performance is measured using the total wall-clock time required to complete the run. The experiments investigate how the number of producers (P), the number of consumers (C), and the buffer size (B) influence overall performance.

---

## 2. Experimental Setup

The simulation evaluates all required combinations of the following three parameters:

- **Producers (P):** 2, 5, 10  
- **Consumers (C):** 2, 5, 10  
- **Buffer Sizes (B):** 3, 10  

This produces **18 total runs**.

Each run outputs:
- Per-consumer statistics  
- Global per-store totals  
- Global per-month totals  
- Final revenue aggregation  
- Total execution time in milliseconds  

All results included in this report were extracted from the provided `sample_output.txt` produced by the program’s `--all` mode.

---

## 3. Observed Execution Times

Table 1 summarizes the execution times measured across all parameter combinations (rounded to the nearest millisecond).

### Table 1. Execution Time by (P, C, B)
| P  | C  | B  | Time (ms) |
|----|----|----|-----------|
| 2  | 2  | 3  | 11534 |
| 2  | 2  | 10 | 11366 |
| 2  | 5  | 3  | 5248  |
| 2  | 5  | 10 | 5072  |
| 2  | 10 | 3  | 3348  |
| 2  | 10 | 10 | 3263  |
| 5  | 2  | 3  | 4081  |
| 5  | 2  | 10 | 3768  |
| 5  | 5  | 3  | 2854  |
| 5  | 5  | 10 | 2691  |
| 5  | 10 | 3  | 2307  |
| 5  | 10 | 10 | 2281  |
| 10 | 2  | 3  | 2617  |
| 10 | 2  | 10 | 2464  |
| 10 | 5  | 3  | 2088  |
| 10 | 5  | 10 | 1996  |
| 10 | 10 | 3  | 1822  |
| 10 | 10 | 10 | 1797  |

---

## 4. Analysis of Results

### 4.1 Effect of Number of Producers (P)

Increasing the number of producers reliably decreases total runtime.

**Reason:**  
More producers generate sales records more quickly. Since each run ends after 1000 items, increased production parallelism directly reduces total execution time.

**Example:**  
- P = 2, C = 2, B = 3 → **~11.5 seconds**  
- P = 5, C = 2, B = 3 → **~4.0 seconds**  
- P = 10, C = 2, B = 3 → **~2.6 seconds**  

This trend reflects a nearly linear improvement, consistent with a producer-limited workload.

---

### 4.2 Effect of Number of Consumers (C)

Increasing the number of consumers also reduces runtime, but benefits diminish once consumption is no longer the bottleneck.

**When C is small (e.g., 2):**
- Consumers cannot keep up.
- Producers frequently block on a full buffer.

**Increasing C to 5 or 10:**
- Consumption rate becomes competitive with production.
- Runtime improves, but with smaller gains beyond C ≥ 5.

---

### 4.3 Effect of Buffer Size (B)

Increasing buffer size from 3 to 10 yields only small performance improvements.

**Reason:**  
Producers and consumers both run frequently with minimal idle time due to short sleep intervals and atomic counters. The buffer rarely becomes the system's bottleneck.

**Impact:** Typically less than **5%**.

Summary:  
- **B = 3:** Slightly more contention.  
- **B = 10:** Smoother operation, minor effect.  

---

### 4.4 Combined Effects

The fastest and slowest configurations illustrate the combined influence of P, C, and B:

- **Fastest:** P = 10, C = 10, B = 10 → **~1797 ms**  
- **Slowest:** P = 2, C = 2, B = 3 → **~11534 ms**  

These results align with expectations:  
- More producers → faster record creation  
- More consumers → faster processing  
- Larger buffer → fewer stalls, but overall minor impact  

---

## 5. Correctness Observations

Across all 18 executions:

- Each run correctly shows *Produced = 1000* and *Consumed = 1000*.  
- No deadlocks, stalls, or hangs occurred.  
- Each consumer printed:
  - A local aggregate  
  - Its top two stores processed  
- Global totals matched the sum of all local statistics.  
- Monthly and per-store totals varied appropriately, consistent with randomized record generation.

All output matched expected simulation behavior.

---

## 6. Conclusions

The experimental findings support the following conclusions:

1. **Increasing the number of producers significantly reduces runtime.**  
2. **Increasing the number of consumers reduces runtime but with diminishing returns.**  
3. **Buffer size has minimal impact** compared to P and C.  
4. The implementation correctly and efficiently models a synchronized bounded-buffer producer–consumer system.  
5. All assignment requirements for Problem 1 were successfully met, including:
   - Correct use of POSIX threads  
   - Proper synchronization with semaphores and mutexes  
   - Shared global statistics  
   - Correct termination after exactly 1000 items  
   - Full evaluation of all 18 parameter combinations  
