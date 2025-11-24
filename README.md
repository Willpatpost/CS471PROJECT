# CS471/571 Project – Fall 2025
Operating System Concepts – Producer–Consumer Simulation & Virtual Memory Management

This repository contains the full implementation of the two required components for the CS471/571 Fall 2025 course project:

1. PRODCONS – A multithreaded producer–consumer simulation using bounded buffers, semaphores, mutexes, shared memory, and statistical aggregation.
2. VMEMMAN – A virtual memory management simulator comparing four page replacement algorithms across multiple page sizes and frame counts.

Both projects include:
- A README.md with build and run instructions
- A Makefile building object files into src/ and executables into bin/
- Documented source code
- Sample input and output
- A lab report summarizing results

------------------------------------------------------------
Project Tree
------------------------------------------------------------
```
CS471PROJECT/
 ├── PRODCONS/
 │    ├── Makefile
 │    ├── README.md
 │    ├── lab_report.md
 │    ├── sample_input.txt
 │    ├── sample_output.txt
 │    ├── bin/
 │    │    └── PRODCONS
 │    └── src/
 │         ├── PRODCONS.c
 │         └── PRODCONS.o
 └── VMEMMAN/
      ├── Makefile
      ├── README.md
      ├── lab_report.md
      ├── sample_input.txt
      ├── sample_output.txt
      ├── bin/
      │    └── VMEMMAN
      └── src/
           ├── VMEMMAN.c
           └── VMEMMAN.o
```
------------------------------------------------------------
Problem 1. PRODCONS – Producer–Consumer Simulation
------------------------------------------------------------
Location:
CS471PROJECT/PRODCONS/

Summary:
Simulates p producers and c consumers sharing a bounded buffer of size b.
Uses:
- POSIX threads
- Semaphores
- Mutex locks
- Shared global counters
- Randomized sales records

The simulation ends after 1000 total records are produced.

Statistics:
- Store-wide total sales
- Month-wise total sales
- Aggregate sales
- Total simulation time

Experiment Matrix:
Producers: 2, 5, 10
Consumers: 2, 5, 10
Buffer sizes: 3, 10
18 total runs (summarized in lab_report.md)

Build:
cd PRODCONS
make

Run:
./bin/prodcons <p> <c> <b>

Clean:
make clean

------------------------------------------------------------
Problem 2. VMEMMAN – Virtual Memory Management Simulator
------------------------------------------------------------
Location:
CS471PROJECT/VMEMMAN/

Summary:
Implements and compares:
- FIFO
- LRU
- MRU
- Optimal

Simulates virtual memory references across:
Page sizes: 512, 1024, 2048 bytes
Frame counts: 4, 8, 12
Nine total configurations (summarized in lab_report.md)

Build:
cd VMEMMAN
make

Run:
./bin/VMEMMAN <input_file> <page_size> <frames>

Example:
./bin/VMEMMAN sample_input.txt 1024 8

Clean:
make clean

------------------------------------------------------------
Notes
------------------------------------------------------------
- Both subprojects contain sample input and output.
- Code is documented and readable.
- Makefiles give clean directory separation (src, bin).
- Executables are placed in bin/.
- Object files are placed in src/.
