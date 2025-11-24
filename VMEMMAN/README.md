# README – Problem 2: Virtual Memory Management Simulator  
Course: CS 471 – Operating Systems  
Project Part 2: VMEMMAN  
Author: William Poston  
Date: 11/24/2025

------------------------------------------------------------
1. Overview
------------------------------------------------------------

This program implements a complete **Virtual Memory Management** simulator that evaluates several page replacement algorithms across multiple memory configurations.

Given an input file of **byte-addressable virtual memory references**, the program:

- Converts each address into a **page number**, using the following page sizes:
  - 512 bytes  
  - 1024 bytes  
  - 2048 bytes  

- Runs the following **four page replacement algorithms**:
  - **FIFO** – First-In First-Out  
  - **LRU** – Least Recently Used  
  - **MRU** – Most Recently Used  
  - **OPT** – Optimal (Belady’s MIN algorithm)  

- Tests all **nine required combinations** of:
  - Page sizes ∈ {512, 1024, 2048}  
  - Frame counts ∈ {4, 8, 12}  

The program then prints:
- Page fault **counts**  
- Page fault **percentages**  
- Algorithm comparison across configurations

This implementation is intentionally **simple, readable, and highly documented**, following the expectations of a CS 471 project submission.

------------------------------------------------------------
2. Files Included
------------------------------------------------------------

VMEMMAN/
│
├── VMEMMAN.c                 (Main program source; fully documented)
│
├── VMEMMAN                   (Compiled executable – Linux/WSL)
│
├── sample_input.txt          (Trace file of byte-addressable virtual addresses)
│
├── sample_output.txt         (Program output using the above input file)
│
└── README.md                 (This file)

------------------------------------------------------------
3. Building the Program
------------------------------------------------------------

To build the project inside the VMEMMAN directory:

```bash
gcc -O2 -Wall -Wextra -o VMEMMAN VMEMMAN.c
```

This compiles:
- `VMEMMAN.c` → `VMEMMAN`  
using GCC with optimizations and warnings enabled.

The program requires:
- Linux (native or WSL2)
- GCC / build-essential package

------------------------------------------------------------
4. Running the Program
------------------------------------------------------------

The simulator reads from:

```
sample_input.txt
```

This file must be in the **same directory** as the executable.

Run the program with:

```bash
./VMEMMAN
```

Behavior:
- The program loads **all virtual addresses** from `sample_input.txt`.
- For each page size (512, 1024, 2048) it computes page numbers.
- For each page size / frame count combination, it runs:
  - FIFO
  - LRU
  - MRU
  - OPT
- Outputs fault percentages in the required 9-line formatted table.

------------------------------------------------------------
5. Program Design Summary
------------------------------------------------------------

Input Handling:
- Reads up to **1,000,000** virtual memory references.
- Skips empty lines; accepts one integer per line.

Page Conversion:
- Uses integer division:
  
  ```
  page_number = virtual_address / page_size
  ```

Replacement Algorithms:
- **FIFO**  
  - Uses a circular queue; evicts the oldest loaded page.

- **LRU**  
  - Maintains last-access timestamps per frame; evicts the least recently used page.

- **MRU**  
  - Opposite of LRU; evicts the most recently used page.
  - Included for comparison (typically performs poorly).

- **OPT (Optimal)**  
  - Looks ahead in the future reference string.
  - Evicts the page whose next use is furthest in the future.
  - Produces the theoretical minimum number of page faults.

Memory Configurations Tested:
- Page sizes: 512, 1024, 2048  
- Frames: 4, 8, 12  

Output:
- For each configuration, prints one line:

```
PageSize=512 Frames=4 | FIFO=80.37%  LRU=80.00%  MRU=93.10%  OPT=56.63%
```

------------------------------------------------------------
6. Sample Output (Excerpt)
------------------------------------------------------------

```
==================== VMEM RESULTS ====================
PageSize=512 Frames=4 | FIFO=80.37%  LRU=80.00%  MRU=93.10%  OPT=56.63%
PageSize=512 Frames=8 | FIFO=61.00%  LRU=60.10%  MRU=91.50%  OPT=34.23%
PageSize=512 Frames=12 | FIFO=42.97%  LRU=42.07%  MRU=88.97%  OPT=21.20%
------------------------------------------------------
PageSize=1024 Frames=4 | FIFO=61.40%  LRU=60.47%  MRU=86.03%  OPT=37.90%
PageSize=1024 Frames=8 | FIFO=23.60%  LRU=22.80%  MRU=81.03%  OPT=11.27%
PageSize=1024 Frames=12 | FIFO=3.57%  LRU=3.57%  MRU=77.27%  OPT=3.40%
------------------------------------------------------
PageSize=2048 Frames=4 | FIFO=26.67%  LRU=26.03%  MRU=73.40%  OPT=13.97%
PageSize=2048 Frames=8 | FIFO=1.90%  LRU=1.90%  MRU=66.30%  OPT=1.73%
PageSize=2048 Frames=12 | FIFO=1.83%  LRU=1.83%  MRU=60.40%  OPT=1.63%
------------------------------------------------------
======================== DONE ========================
```

Full output is included in `sample_output.txt`.

------------------------------------------------------------
7. Notes
------------------------------------------------------------

- Problem 2 **requires** an input file (unlike Problem 1).  
- The simulator does *not* generate synthetic access patterns; it replays `sample_input.txt`.  
- OPT serves as a baseline to compare FIFO/LRU/MRU effectiveness.  
- MRU is intentionally included to show how certain policies degrade performance.  
- This program is independent from **Part 1 (PRODCONS)**.
