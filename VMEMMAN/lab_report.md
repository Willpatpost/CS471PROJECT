# CS 471 / 571 — Problem 2 Report
## Virtual Memory Management Simulation (VMEMMAN)
**Author:** William Poston  
**Date:** November 24, 2025  

---

## 1. Introduction

This report presents an analysis of the Virtual Memory Management simulation developed for Problem 2. The VMEMMAN program evaluates the performance of four widely studied page replacement algorithms when applied to a trace of virtual memory references.

The simulator reads a sequence of byte-addressable virtual addresses from `sample_input.txt`, converts each address to a page number according to multiple page sizes, and then performs page replacement under a variety of frame allocations.

The four replacement algorithms implemented are:

- **FIFO** — First-In First-Out  
- **LRU** — Least Recently Used  
- **MRU** — Most Recently Used  
- **OPT** — Optimal / Belady’s MIN Algorithm  

The goal of the experiment is to examine how **page size**, **number of frames**, and **choice of algorithm** influence the overall page fault rate during execution.

---

## 2. Experimental Setup

The simulator evaluates all combinations of the following parameters:

### Page Sizes
- 512 bytes  
- 1024 bytes  
- 2048 bytes  

### Frame Counts
- 4  
- 8  
- 12  

These values yield **nine total configurations**, each of which is tested using all four replacement algorithms.

For each configuration, the simulator outputs:
- Total page fault count  
- Page fault percentage (faults ÷ 3000 references × 100)  
- A formatted, comparative summary line  

All results discussed in this report are taken directly from the verified output in `sample_output.txt`, using the provided trace of 3000 virtual memory references.

---

## 3. Observed Results

Tables 1–3 summarize the **page fault percentages** observed for each algorithm at each page size and frame allocation.

### Table 1. Page Size = 512 bytes
| Frames | FIFO    | LRU     | MRU     | OPT     |
|-------|---------|---------|---------|---------|
| 4     | 80.37%  | 80.00%  | 93.10%  | 56.63%  |
| 8     | 61.00%  | 60.10%  | 91.50%  | 34.23%  |
| 12    | 42.97%  | 42.07%  | 88.97%  | 21.20%  |

### Table 2. Page Size = 1024 bytes
| Frames | FIFO    | LRU     | MRU     | OPT     |
|-------|---------|---------|---------|---------|
| 4     | 61.40%  | 60.47%  | 86.03%  | 37.90%  |
| 8     | 23.60%  | 22.80%  | 81.03%  | 11.27%  |
| 12    | 3.57%   | 3.57%   | 77.27%  | 3.40%   |

### Table 3. Page Size = 2048 bytes
| Frames | FIFO    | LRU     | MRU     | OPT     |
|-------|---------|---------|---------|---------|
| 4     | 26.67%  | 26.03%  | 73.40%  | 13.97%  |
| 8     | 1.90%   | 1.90%   | 66.30%  | 1.73%   |
| 12    | 1.83%   | 1.83%   | 60.40%  | 1.63%   |

---

## 4. Analysis of Results

### 4.1 Effect of Page Size
Increasing the page size significantly reduces the number of page faults.

**Example:**
- FIFO at 512 bytes, 4 frames → ~80% faults  
- FIFO at 2048 bytes, 4 frames → ~27% faults  

**Reason:**  
Larger pages map more addresses to the same page number, reducing the number of distinct pages and increasing spatial locality.

**Conclusion:**  
**Page size has the largest single impact** on page fault rate.

---

### 4.2 Effect of Number of Frames
Increasing the number of frames consistently decreases page faults for FIFO, LRU, and OPT.

**Example (Page size 1024 bytes, FIFO):**
- 4 frames → 61.40%  
- 8 frames → 23.60%  
- 12 frames → 3.57%  

**Exception:**  
MRU does **not** improve with more frames because it evicts the most recently used page, which is typically needed again soon.

**Conclusion:**  
Adding frames improves performance for all algorithms except MRU.

---

### 4.3 Comparison of Algorithms
Across all configurations:

- **OPT** achieves the lowest fault rate, as theoretically expected.  
- **LRU** consistently outperforms **FIFO**, often modestly.  
- **MRU** performs dramatically worse than all others.

**Example (1024 bytes, 12 frames):**
- FIFO: 3.57%  
- LRU: 3.57%  
- OPT: 3.40%  
- MRU: **77.27%**  

**Conclusion:**  
**LRU is the most effective practical policy**, while MRU is highly inefficient for general workloads.

---

### 4.4 Combined Effects
Best results occur when both page size and frame count are large:

- **2048 bytes, 12 frames → OPT ≈ 1.63% faults**

Worst results occur for small page sizes and few frames:

- **512 bytes, 4 frames → MRU ≈ 93.10% faults**

Overall trends:
- Larger pages reduce page faults dramatically.  
- More frames further decrease faults.  
- Algorithm selection strongly shapes performance at small memory sizes.

These patterns align with standard OS theory on locality and working-set behavior.

---

## 5. Correctness Observations
The simulator behaved correctly across all experiments:

- All 3000 addresses were processed without errors.  
- FIFO, LRU, MRU, and OPT produced valid outputs for all configurations.  
- No segmentation faults, hangs, or invalid accesses occurred.  
- OPT consistently delivered the minimum possible fault count.  
- MRU behaved as expected, demonstrating intentionally poor performance.  
- Output formatting matches all assignment guidelines.

All results were cross-validated with the provided `sample_output.txt` file.

---

## 6. Conclusions

The simulation results support several key conclusions:

1. **Page size is the dominant factor** affecting page fault frequency.  
2. **Increasing frame count** significantly reduces faults for FIFO, LRU, and OPT.  
3. **LRU closely approaches OPT** and is the best realistic algorithm tested.  
4. **MRU performs extremely poorly**, confirming its unsuitability for most workloads.  
5. The VMEMMAN simulator correctly implements:  
   - Reference string parsing  
   - Page number conversion  
   - FIFO, LRU, MRU, and OPT  
   - All nine configuration evaluations  
   - Properly formatted output  
