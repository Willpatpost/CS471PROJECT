============================================================
CS 471 / 571 – Problem 2 Report  
Virtual Memory Management Simulation (VMEMMAN)  
Author: William Poston  
Date: 11/24/2025
============================================================

1. Introduction
------------------------------------------------------------
This report summarizes the results of the Virtual Memory Management
simulation specified in Problem 2. The VMEMMAN program evaluates
the performance of four classical page replacement algorithms when
executing a sequence of virtual memory references.

The program reads a trace of byte-addressable virtual addresses from
`sample_input.txt`, converts each address into a page number using
multiple page sizes, and then simulates page replacement under different
frame allocations.

The four algorithms tested are:

- FIFO  — First-In First-Out  
- LRU   — Least Recently Used  
- MRU   — Most Recently Used  
- OPT   — Optimal (Belady’s MIN algorithm)  

The goal of this analysis is to observe how:

- Page size
- Number of frames
- Choice of replacement algorithm

affect the resulting page fault rates.


2. Experimental Setup
------------------------------------------------------------
The simulator evaluates all required combinations of:

**Page Sizes:**  
- 512 bytes  
- 1024 bytes  
- 2048 bytes  

**Frame Counts:**  
- 4  
- 8  
- 12  

This yields a total of **9 runs**, each of which executes all four
replacement algorithms.

For each run, the program prints:

- Page fault count  
- Page fault percentage (faults ÷ total references × 100)  
- Comparative results on a single line  

The results reported below correspond directly to the real output found
in `sample_output.txt`, produced by the VMEMMAN program using the
provided trace file of 3000 virtual memory references.


3. Observed Results
------------------------------------------------------------
The tables below summarize the observed **page fault percentages** for
each combination of page size and frame count.

### Page Size = 512 bytes
Frames | FIFO     | LRU      | MRU      | OPT
------ | -------- | -------- | -------- | --------
4      | 80.37%   | 80.00%   | 93.10%   | 56.63%
8      | 61.00%   | 60.10%   | 91.50%   | 34.23%
12     | 42.97%   | 42.07%   | 88.97%   | 21.20%

### Page Size = 1024 bytes
Frames | FIFO     | LRU      | MRU      | OPT
------ | -------- | -------- | -------- | --------
4      | 61.40%   | 60.47%   | 86.03%   | 37.90%
8      | 23.60%   | 22.80%   | 81.03%   | 11.27%
12     | 3.57%    | 3.57%    | 77.27%   | 3.40%

### Page Size = 2048 bytes
Frames | FIFO     | LRU      | MRU      | OPT
------ | -------- | -------- | -------- | --------
4      | 26.67%   | 26.03%   | 73.40%   | 13.97%
8      | 1.90%    | 1.90%    | 66.30%   | 1.73%
12     | 1.83%    | 1.83%    | 60.40%   | 1.63%


4. Analysis of Results
------------------------------------------------------------

4.1 Effect of Page Size  
Increasing the page size significantly reduces the number of page
faults.

Examples:
- At 512 bytes with 4 frames, FIFO faults ≈ 80%
- At 2048 bytes with 4 frames, FIFO faults ≈ 27%

Reason:
Larger pages map a wider range of addresses onto the same page number.
This reduces total unique pages and improves locality, greatly decreasing
fault frequency.

Conclusion:
**Page size has the largest impact** of all variables tested.

---

4.2 Effect of Number of Frames  
Increasing the number of frames always reduces page faults for FIFO,
LRU, and OPT.

Examples (page size 1024 bytes):
- 4 frames → FIFO = 61.40%
- 8 frames → FIFO = 23.60%
- 12 frames → FIFO = 3.57%

Reason:
More frames accommodate more working-set pages, reducing evictions.

Exception:
MRU does *not* monotonically improve with more frames due to its
counterproductive eviction policy.

Conclusion:
More frames = fewer faults, except for MRU.

---

4.3 Comparison of Algorithms  
Across all experiments:

**OPT** produces the lowest fault rate in every case  
(as expected: it is the theoretical best possible algorithm).

**LRU** consistently outperforms **FIFO**, often by a small margin.

**MRU** performs worst by a wide margin:
- Its fault rate is extremely high across all configurations.
- MRU evicts the most recently used page, which is typically the one
  that will be used again soon.
- This makes MRU unsuitable for most workloads.

Example (page size 1024, 12 frames):
- FIFO: 3.57%
- LRU: 3.57%
- OPT: 3.40%
- MRU: **77.27%**

Conclusion:
**LRU is a practical and effective policy, while MRU is highly inefficient.**

---

4.4 Combined Effects  
The very best results occur when both page size and frame count are large:

- PageSize = 2048, Frames = 12 → OPT ≈ 1.63%

The worst results occur with small page size and few frames:

- PageSize = 512, Frames = 4 → MRU ≈ 93.10%

Overall trends:
- Larger pages drastically reduce fault frequency.
- More frames provide further reductions.
- Algorithm choice strongly affects results, especially for small memory sizes.

These observations match standard operating systems theory regarding
working-set behavior and replacement policy efficiency.


5. Correctness Observations
------------------------------------------------------------
From review of all simulation runs:

- The program processed all 3000 address references without error.
- FIFO, LRU, MRU, and OPT all executed properly for each configuration.
- No segmentation faults, invalid memory accesses, or hangs occurred.
- OPT always produced the minimal fault count, as expected.
- MRU behaved as anticipated, demonstrating poor performance and serving
  as a pedagogical contrast to LRU/FIFO/OPT.
- The formatted output matches all requirements of the assignment.

All results were verified using the included `sample_output.txt` file.


6. Conclusions
------------------------------------------------------------
The experimental results lead to several clear conclusions:

1. **Page size has the strongest impact** on page fault rates.  
2. **Increasing frame count** consistently improves performance for FIFO, LRU, and OPT.  
3. **LRU closely approximates OPT** and is generally the best practical algorithm.  
4. **MRU performs extremely poorly**, confirming its unsuitability for general workloads.  
5. The VMEMMAN simulator correctly implements all assignment requirements, including:
   - Correct reference string handling  
   - Proper page number conversion  
   - Implementation of FIFO, LRU, MRU, and OPT  
   - Evaluation across 9 required configurations  
   - Clear and formatted output suitable for reporting  

This concludes the report for Problem 2.
------------------------------------------------------------
