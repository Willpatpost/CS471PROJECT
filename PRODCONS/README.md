# README – Problem 1: Producer–Consumer System with Statistics  
Course: CS 471 – Operating Systems  
Project Part 1: PRODCONS  
Author: William Poston  
Date: 11/23/2025

------------------------------------------------------------
1. Overview
------------------------------------------------------------

This program implements a multi-threaded Producer–Consumer simulation using:

- POSIX threads (pthread)
- POSIX semaphores (sem_t)
- Mutex locks for buffer protection
- A bounded circular buffer
- Atomic counters for shared state
- Per-consumer local statistics and global merged statistics

Each producer generates synthetic retail sales records.
Each consumer removes records from the shared buffer and computes statistics.

The simulation stops when a total of 1000 records have been produced across all producers.

After the run completes, the program prints:

- Per-consumer summaries  
- Global totals by store  
- Global totals by month  
- Global aggregate sales  
- Total run time (milliseconds)

The program also supports an automated mode (`--all`) that runs all 18 assignment-required test configurations.

------------------------------------------------------------
2. Files Included
------------------------------------------------------------

PRODCONS/
│
├── src/
│   └── PRODCONS.c            (Main program source)
│
├── bin/
│   └── PRODCONS              (Compiled executable)
│
├── Makefile                  (Build script)
│
├── sample_output.txt         (Output from all 18 required runs)
│
└── README.md                 (This file)

------------------------------------------------------------
3. Building the Program
------------------------------------------------------------

From inside the PRODCONS directory:

    make

This compiles:
- src/PRODCONS.c → bin/PRODCONS  
using gcc, -O2 optimizations, and pthread support.

To clean:

    make clean

------------------------------------------------------------
4. Running the Program
------------------------------------------------------------

A. Single-Run Mode
------------------

    ./bin/PRODCONS <producers> <consumers> <buffer_size>

Example:

    ./bin/PRODCONS 2 2 3

Arguments:
- <producers>    Number of producer threads (P)
- <consumers>    Number of consumer threads (C)
- <buffer_size>  Bounded buffer size (B)

Behavior:
- Producers generate records until 1000 total items are created.
- Consumers remove items until production is complete and all items are consumed.
- Each consumer prints local statistics.
- Global statistics and timing are printed after all consumers finish.


B. Automated Mode (All 18 Runs)
-------------------------------

Required combinations:
- Producers P ∈ {2, 5, 10}
- Consumers C ∈ {2, 5, 10}
- Buffer sizes B ∈ {3, 10}

To run all 18 automatically:

    ./bin/PRODCONS --all

Full output for these runs is provided in sample_output.txt.

------------------------------------------------------------
5. Program Design Summary
------------------------------------------------------------

Synchronization:
- sem_t empty : tracks open slots in the buffer
- sem_t full  : tracks filled slots
- pthread_mutex_t qmtx : protects circular buffer and indices

Atomic shared variables:
- produced_total
- consumed_total
- done flag (indicates producers have finished)

Producers:
- Generate random records (date, store, register, amount)
- Use semaphores to wait for empty space
- Insert into circular buffer
- Stop after total production reaches 1000

Consumers:
- Wait for available items using semaphores
- Remove items from buffer
- Accumulate local stats (per store, per month, aggregate)
- After completion, merge results into global stats
- Print a per-consumer summary

Global statistics:
- Total sales per store
- Total sales per month
- Aggregate revenue across all data

------------------------------------------------------------
6. Sample Output (Excerpt)
------------------------------------------------------------

--- Consumer 1 summary ---
Local aggregate: 242114.64
Top store: 1 total=177031.43
Next store: 2 total=65083.21

--- Consumer 0 summary ---
Local aggregate: 259722.59
Top store: 2 total=176009.96
Next store: 1 total=83712.63

====================================
Produced=1000  Consumed=1000  Time=569.38 ms

==== Overall Per-Store Totals ====
Store  1: 260744.06
Store  2: 241093.17

==== Overall Per-Month Totals ====
Jan: 47001.25
Feb: 39549.36
Mar: 44526.74
...

==== Overall Aggregate ====
TOTAL: 501837.23

Full 18-run sample is in sample_output.txt.

------------------------------------------------------------
7. Notes
------------------------------------------------------------

- Part 1 does NOT use any external input file.
- All sales data is randomly generated.
- The --all mode is provided to simplify grading.
- Part 2 (VMEMMAN) is in its own folder and independent from this assignment.
