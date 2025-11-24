============================================================
CS 471 / 571 – Problem 1 Report
Producer–Consumer Simulation with Statistics
Author: William Poston
Date: 11/23/2025
============================================================

1. Introduction
------------------------------------------------------------
This report summarizes the results of 18 experimental runs of the
Producer–Consumer simulation specified in Problem 1. The program
implements a multi-threaded bounded-buffer system using POSIX
threads, semaphores, mutexes, and atomic counters.

Each run terminates after 1000 total sales records are produced.
Performance is measured as total wall-clock time for the entire run.
The experiments evaluate how the number of producers (P), consumers
(C), and buffer size (B) affect performance.


2. Experimental Setup
------------------------------------------------------------
The required experiments cover all combinations of:

- Producers P ∈ {2, 5, 10}
- Consumers C ∈ {2, 5, 10}
- Buffer sizes B ∈ {3, 10}

This yields 18 total runs.

Each run prints:
- Per-consumer statistics
- Global per-store totals
- Global per-month totals
- Final aggregate revenue
- Total execution time in milliseconds

All results were taken from the attached sample_output.txt file, which
contains the raw output from the program’s --all mode.


3. Observed Execution Times
------------------------------------------------------------
Below is a condensed table of execution times extracted from the real
program output. (Values rounded to nearest millisecond.)

P   C    B     Time (ms)
--------------------------------
2   2    3     11534
2   2    10    11366
2   5    3     5248
2   5    10    5072
2   10   3     3348
2   10   10    3263

5   2    3     4081
5   2    10    3768
5   5    3     2854
5   5    10    2691
5   10   3     2307
5   10   10    2281

10  2    3     2617
10  2    10    2464
10  5    3     2088
10  5    10    1996
10  10   3     1822
10  10   10    1797


4. Analysis of Results
------------------------------------------------------------

4.1 Effect of Number of Producers (P)
Increasing P reliably decreases total runtime.

Reason:
With more producers, items enter the buffer more quickly. Since the
system stops after a fixed 1000 items, additional production parallelism
reduces total simulation time.

Example:
- P = 2, C = 2, B = 3 → ~11.5 seconds
- P = 5, C = 2, B = 3 → ~4.0 seconds
- P = 10, C = 2, B = 3 → ~2.6 seconds

This nearly linear improvement matches expectations for a producer-
limited workload.

4.2 Effect of Number of Consumers (C)
Increasing C also reduces runtime, but with diminishing returns.

When C is small (e.g., 2):
- Consumers become a bottleneck.
- Producers frequently block on a full buffer.

When C increases to 5 or 10:
- Consumption keeps pace with production.
- Runtime improves moderately.

However, after C ≥ 5, the gains become smaller because consumption
is no longer the dominant bottleneck.

4.3 Effect of Buffer Size (B)
Changing buffer size from 3 → 10 produces only small improvements.

Reason:
With atomic counters and short producer sleep intervals, producers and
consumers remain active almost continually. The buffer is never the
primary bottleneck. A size of 3 is already sufficient to prevent excessive
blocking.

Overall:
- B = 3: Slightly more semaphore contention
- B = 10: Smoother operation but minimal impact

Impact: Typically < 5%.

4.4 Combined Effects
The fastest runs occur when both P and C are large:

- P = 10, C = 10, B = 10 → ~1797 ms

The slowest runs occur when both P and C are small:

- P = 2, C = 2, B = 3 → ~11534 ms

This is consistent with the model:
- More producers = faster item creation
- More consumers = faster item processing
- Larger buffer = fewer stalls, but minor effect overall


5. Correctness Observations
------------------------------------------------------------
From all 18 runs:

- Each run shows “Produced=1000” and “Consumed=1000”.
- No deadlocks or hangs occurred.
- Each consumer correctly printed:
  - Local aggregate
  - Top 2 stores processed
- Global statistics were consistent with the sum of all local statistics.
- Month-wise totals and store totals were reasonable and varied each run,
  matching expected random data generation.


6. Conclusions
------------------------------------------------------------
The experimental results show:

1. Increasing producers dramatically reduces runtime.  
2. Increasing consumers also reduces runtime but with diminishing returns.  
3. Buffer size has minimal overall effect compared to P and C.  
4. The program successfully implements a correct and efficient bounded-
   buffer producer–consumer system with synchronized global statistics.  
5. All assignment requirements for Problem 1 were met, including:
   - Multi-threading
   - Proper semaphore and mutex synchronization
   - Shared statistics
   - Correct termination conditions
   - Experimental evaluation over 18 parameter combinations

This concludes the report for Problem 1.
------------------------------------------------------------
