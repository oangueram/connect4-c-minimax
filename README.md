# High-Performance Connect-4 AI Engine (C99)

[![Language](https://img.shields.io/badge/Language-C99-00599C.svg?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![Build](https://img.shields.io/badge/Build-GCC%20%7C%20Make-orange.svg)](https://gcc.gnu.org/)
[![Memory](https://img.shields.io/badge/Memory-Zero%20Leaks%20%7C%20O(d)-brightgreen.svg)]()
[![Performance](https://img.shields.io/badge/Speedup-%3E200x%20vs%20Naive-blueviolet.svg)]()

A high-performance, memory-efficient **Connect-4 game engine and AI** implemented from scratch in pure **C99**. The engine operates on an expanded $8 \times 8$ board ($N=8$) and searches up to **10 plies deep in real-time (~1.5 s, 334k nodes)** by leveraging **Alpha-Beta pruning**, **center-out move ordering**, **64-bit Zobrist Hashing with Transposition Tables**, and **gravity-aware tactical heuristics**.

Designed with strict software engineering principles: **modular architecture**, **zero memory leaks**, and **zero mutable global state** for search telemetry.

---

## Executive Summary & Engineering Highlights

| Metric | Naive Minimax | Optimized Engine (Ours) | Engineering Impact |
| :--- | :--- | :--- | :--- |
| **Depth 8 Search Time** | ~54.0 s | **0.268 s** (41k nodes) | **$200\times$ Speedup** via early Alpha-Beta cutoffs & cache hits |
| **Depth 10 Search Time** | > 3,600 s (1 hr) | **1.518 s** (334k nodes) | Reaches deep tactical foresight in real-time interactive latency |
| **Peak Memory Footprint** | ~500 MB (Full tree) | **< 1 MB** | Ephemeral node lifecycle: $\text{allocations} == \text{deallocations}$ |
| **Cache Lookup Overhead** | $O(N^2)$ board comparison | **$O(1)$ XOR lookup** | 64-bit bitwise Zobrist hashing |
| **Global State** | Common anti-pattern | **0 global telemetry variables** | Thread-safe, reentrant profiling via stack-passed pointers |

---

## System Architecture

```text
                     +-----------------------------+
                     |        Game Loop (C99)      |
                     +--------------+--------------+
                                    |
                                    v
                     +-----------------------------+
                     |    ferMinimax() (Root Ply)  |
                     |  - Allocates local stats    |
                     |  - High-res timer (clock)   |
                     +--------------+--------------+
                                    |
                                    v
                     +-----------------------------+
                     |    avaluarMinimax() (Tree)  | <---------+
                     +--------------+--------------+           |
                                    |                          |
        +---------------------------+-----------------------+  |
        |                           |                       |  |
        v                           v                       v  |
+-------------------+      +-------------------+      +--------+----------+
| Zobrist TT Lookup |      | Alpha-Beta Search |      | Heuristic Engine  |
| - O(1) XOR Hash   |      | - Move Ordering   |      | - Center Weight   |
| - Bound Caching   |      | - Branch Pruning  |      | - Gravity Support |
| - Depth Eviction  |      | - Bound Tracking  |      | - Threat Detection|
+-------------------+      +-------------------+      +-------------------+
```

---

## Technical Deep-Dive

### 1. 64-Bit Zobrist Hashing & Transposition Table
- **$O(1)$ Incremental State Hashing**: Evaluates board state updates algebraically in $O(1)$ time using bitwise XOR ($\oplus$):

  $$\text{hash}_{\text{child}} = \text{hash}_{\text{parent}} \oplus \text{Zobrist}[\text{row}][\text{col}][\text{piece}]$$

  Leverages the algebraic property $x \oplus y \oplus y = x$ across an $8 \times 8 \times 2$ matrix of cryptographically spaced 64-bit random keys.
- **Direct-Mapped Cache**: Direct-mapped table with $2^{20} = 1,048,576$ entries (`MIDA_TAULA`), using ultra-fast bitwise indexing (`hash % MIDA_TAULA`).
- **Mathematical Bound Classification**: Distinguishes exact scores from cutoff approximations:
  - `TIPUS_EXACTE` (0): Complete search value without prune.
  - `TIPUS_COTA_INFERIOR` (1): Beta cutoff ($\ge \beta_{\text{original}}$), lower bound.
  - `TIPUS_COTA_SUPERIOR` (2): Alpha cutoff or fail-low ($\le \alpha_{\text{original}}$), upper bound.
- **Depth-Preferred Eviction**: Replaces entries when $\text{depth}_{\text{remaining}} \ge \text{depth}_{\text{cached}}$, protecting high-investment branch calculations while discarding obsolete states.

### 2. Alpha-Beta Pruning with Center-Out Move Ordering
- Alpha-Beta reduces the theoretical branching factor from $b$ towards $\sqrt{b}$, transforming computational complexity from $O(b^d)$ to $O(b^{d/2})$.
- **Dynamic Exploration Ordering**: Prioritizes central columns first:

  $$\text{Priority: } [3, 4, 2, 5, 1, 6, 0, 7]$$

  Central columns maximize connectable vectors (horizontal, vertical, and both diagonals). Evaluating strong moves first triggers immediate $\beta$-cutoffs on up to **90% of sub-trees**.

### 3. Ephemeral Dynamic Memory Lifecycle
- Instead of building a static multi-gigabyte tree in memory, the engine dynamically generates nodes on the fly:
  - Each child node is allocated (`malloc`), evaluated recursively, and immediately freed (`free`).
  - Total allocated tree memory scales with search depth $O(d)$, strictly maintaining **zero memory leaks** ($\text{mallocs} == \text{frees}$).

### 4. Domain-Specific Tactical Heuristics
- **Positional Gravity Support (`teSuport`)**: Connect-4 pieces cannot float. The evaluation verifies whether an empty key slot is supported from below. Immediate tactical threats are prioritized, while unsupported slots are identified as future traps.
- **Double-Threat (*Fork*) Detection**: Identifies states containing multiple simultaneous 3-in-a-row threats ($\ge 2$), assigning a $+5000$ reward for unstoppable forced-win paths.
- **Depth-Discounted Terminal States**: Victories are scored as $\pm (1,000,000 - \text{depth} \times 10,000)$, mathematically driving the AI to pursue the shortest path to checkmate and delay opponent victories.

### 5. Clean Engineering: Zero Global State Telemetry
- Avoids mutable global variables by encapsulating live execution profiling inside a stack-allocated structure:
  ```c
  typedef struct {
      long long int nodesExplorats;
      long long int repetitsTT;
  } Estadistiques;
  ```
- Passed down the recursion chain strictly by reference (`Estadistiques *stats`), ensuring reentrant, thread-safe, and modular code.

---

## Benchmarks & Performance Analysis

Measured on an $8 \times 8$ board on an Intel Core i7 system (Opening game phase, 8 available legal columns):

| Search Depth | Effective Plies | Naive Minimax (No Pruning) | Alpha-Beta (Standard) | Alpha-Beta + Move Ordering | Engine (Alpha-Beta + Move Ordering + TT)* | Cumulative Speedup |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **2** | 1 AI / 1 Human | < 0.001 s (64 nodes) | < 0.001 s (40 nodes) | < 0.001 s (30 nodes) | **< 0.001 s** (30 nodes) | **Baseline** |
| **4** | 2 AI / 2 Human | 0.015 s (4.1k nodes) | 0.006 s (1.5k nodes) | 0.002 s (820 nodes) | **< 0.001 s** (675 nodes) | **$15\times$** |
| **6** | 3 AI / 3 Human | 0.850 s (262k nodes) | 0.120 s (35k nodes) | 0.045 s (12k nodes) | **0.034 s** (6.1k nodes) | **$25\times$** |
| **8** | 4 AI / 4 Human | ~54.0 s (16.7M nodes) | 1.850 s (520k nodes) | 0.450 s (80k nodes) | **0.268 s** (41.2k nodes) | **$200\times$** |
| **10** | 5 AI / 5 Human | > 3,600 s (1.07B nodes) | > 30 s (~8.5M nodes) | ~3.8 s (~950k nodes) | **1.518 s** (334.2k nodes) | **$> 2,000\times$** |

*\* Benchmark Methodology Note: Real-time measurements were executed on an Intel Core i7 processor running Windows 11 (compiled with GCC 13.2.0, `-O2` release optimization). Timings reflect a representative opening position on an $8 \times 8$ grid, measured via high-resolution `clock()` profiling. Naive Minimax timings for depths 8 and 10 are derived from theoretical game-tree state counts ($8^8$ and $8^{10}$ nodes) at the engine's baseline leaf evaluation throughput (~310,000 nodes/second).*

---

## Project Structure

```text
├── 4enratlla.h       # Board dimensions (N=8) and core game prototypes
├── 4enratlla.c       # Main game loop, board I/O, gravity logic, win detection
├── minimax.h         # Node definitions, TT cache entry, Stats struct, prototypes
├── minimax.c         # Alpha-Beta engine, Zobrist TT, heuristic evaluation
├── Makefile          # Automated compilation with dependency tracking
└── README.md         # Engineering documentation and architectural benchmarks
```

---

## Build & Run

### Prerequisites
- GCC / Clang supporting **C99**
- Make (`mingw32-make` on Windows, `make` on Linux/macOS)

### Compilation
```bash
# Compile optimized release binary (-O2 with strict warnings)
make

# Or compile directly:
gcc -Wall -Wextra -std=c99 -O2 4enratlla.c minimax.c -o 4enratlla.exe
```

### Run
```bash
# Windows
.\4enratlla.exe

# Linux / macOS
./4enratlla.exe
```

### Live Terminal Telemetry Output
```text
Torn de la màquina.
[PROFUNDITAT 8] TEMPS:0.268 | NODES EXPLORATS 41289 | REUTILITZACIONS TT:3283
La màquina ha tirat a la columna 3
```

---

## Skills & Core Competencies Demonstrated

- **Systems Programming in C**: Pointer arithmetic, stack vs. heap allocation, memory leak prevention, C99 standards.
- **Algorithms & Optimization**: Branch-and-bound (Alpha-Beta), Minimax game trees, Move Ordering heuristics, bitwise operations.
- **Cache Architecture**: Direct-mapped hash tables, hash collisions, algebraic Zobrist hashing, cache replacement policies.
- **Clean Software Architecture**: Modular design, thread-safe reentrant functions, zero mutable global state.

---

## Author

**Oriol Anguera Milà**  
B.S. in Mathematics — *Universitat Autònoma de Barcelona (UAB)*  
[GitHub Profile](https://github.com/oangueram)
