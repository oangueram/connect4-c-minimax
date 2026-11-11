# Connect-4 AI Engine in C (High Performance)

[![Language](https://img.shields.io/badge/Language-C99-00599C.svg?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![Build](https://img.shields.io/badge/Build-GCC%20%7C%20Make-orange.svg)](https://gcc.gnu.org/)
[![Memory](https://img.shields.io/badge/Memory-Zero%20Leaks-brightgreen.svg)]()
[![Performance](https://img.shields.io/badge/Speedup-200x%20vs%20Naive-blueviolet.svg)]()

A fast, memory-efficient **Connect-4 AI engine** built from scratch in **C99**. The engine plays on an expanded 8×8 board and searches up to **10 moves ahead in real-time (~1.5 s)** using **Alpha-Beta pruning**, **center-first move ordering**, **64-bit Zobrist Hashing with Transposition Tables**, and **gravity-aware heuristics**.

---

## Performance Highlights

| Metric | Naive Minimax | Optimized Engine (Ours) | Impact |
| :--- | :--- | :--- | :--- |
| **Depth 8 Search Time** | ~54.0 s | **0.268 s** (41k nodes) | **200x Speedup** via Alpha-Beta pruning & cache hits |
| **Depth 10 Search Time** | > 3,600 s (1 hr) | **1.518 s** (334k nodes) | Real-time play looking 10 plies ahead |
| **Peak Memory Footprint** | ~500 MB (Full tree) | **< 1 MB** | Ephemeral node lifecycle (allocates & frees on the fly) |
| **Cache Lookup Speed** | Full board scan | **O(1) XOR lookup** | 64-bit Zobrist hash table (2²⁰ positions) |

---

## Core Optimizations

### 1. Alpha-Beta Pruning & Center-First Ordering
- Evaluates the game tree while eliminating branches that cannot influence the final decision.
- Explores **central columns first** (`3, 4, 2, 5, 1, 6, 0, 7`). Because central positions offer more winning lines, the engine finds strong moves immediately and triggers fast cutoffs on up to **90% of sub-branches**.

### 2. Transposition Tables with 64-bit Zobrist Hashing
- Many move sequences lead to the exact same board state (transpositions). Instead of recalculating them, the engine caches evaluated positions in a 2²⁰ entry hash table (~1 million slots).
- Board hashes update in O(1) time per move using bitwise XOR ($\oplus$):

$$
\text{hash}_{\text{child}} = \text{hash}_{\text{parent}} \oplus \text{Zobrist}[\text{row}][\text{col}][\text{piece}]
$$

- Caches exact values as well as mathematical upper/lower bounds from cutoffs, replacing shallower analyses with deeper ones.

### 3. Gravity-Aware Tactical Heuristics
- **Gravity Support Check**: Connect-4 pieces cannot float. The engine verifies whether key squares have support underneath, distinguishing between **immediate threats** (playable right now) and **future traps** (which neither player should trigger prematurely).
- **Double Threats (*Forks*)**: Identifies and rewards positions with multiple simultaneous winning threats that the opponent cannot block in a single turn.
- **Depth-Discounted Wins**: Prefers winning as quickly as possible and delaying opponent wins.

### 4. Real-Time Telemetry
On every turn, the engine displays real-time search statistics:
- Time spent thinking
- Total nodes explored
- Cache hits (subtrees skipped thanks to the Transposition Table)

```text
Torn de la màquina.
[PROFUNDITAT 8] TEMPS:0.268 | NODES EXPLORATS 41289 | REUTILITZACIONS TT:3283
La màquina ha tirat a la columna 3
```

---

## Benchmark Comparison

Measured on an 8×8 board (opening position, 8 available columns):

| Search Depth | Effective Plies | Naive Minimax (No Pruning) | Alpha-Beta (Standard) | Alpha-Beta + Move Ordering | Engine (Alpha-Beta + Move Ordering + TT) | Speedup vs Naive |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **2** | 1 AI / 1 Human | < 0.001 s (64 nodes) | < 0.001 s (40 nodes) | < 0.001 s (30 nodes) | **< 0.001 s** (30 nodes) | Baseline |
| **4** | 2 AI / 2 Human | 0.015 s (4.1k nodes) | 0.006 s (1.5k nodes) | 0.002 s (820 nodes) | **< 0.001 s** (675 nodes) | **15x** |
| **6** | 3 AI / 3 Human | 0.850 s (262k nodes) | 0.120 s (35k nodes) | 0.045 s (12k nodes) | **0.034 s** (6.1k nodes) | **25x** |
| **8** | 4 AI / 4 Human | ~54.0 s (16.7M nodes) | 1.850 s (520k nodes) | 0.450 s (80k nodes) | **0.268 s** (41.2k nodes) | **200x** |
| **10** | 5 AI / 5 Human | > 3,600 s (1.07B nodes) | > 30 s (~8.5M nodes) | ~3.8 s (~950k nodes) | **1.518 s** (334.2k nodes) | **> 2,000x** |

> **Benchmark Note:** Measured on an Intel Core i7 system running Windows 11 with GCC (`-O2`). Timings reflect an opening board state on an 8×8 grid. Naive Minimax timings for depths 8 and 10 are calculated from theoretical tree sizes (8⁸ and 8¹⁰ nodes) at the engine's throughput (~310,000 nodes/sec).

---

## Project Structure

```text
├── 4enratlla.h       # Board definitions (N=8) and game prototypes
├── 4enratlla.c       # Game loop, user interface, gravity logic, win detection
├── minimax.h         # Minimax prototypes, TT structures, and statistics
├── minimax.c         # Alpha-Beta search, Zobrist TT, heuristics, and telemetry
├── Makefile          # Build script
└── README.md         # Documentation
```

---

## Build & Run

### Prerequisites
- GCC or Clang (C99 support)
- Make (`mingw32-make` on Windows, `make` on Linux/macOS)

### Compile & Execute
```bash
# Compile with optimizations
make

# Run the game
# Windows:
.\4enratlla.exe

# Linux / macOS:
./4enratlla.exe
```

---

## Author

**Oriol Anguera**  
B.S. in Mathematics — *Universitat Autònoma de Barcelona (UAB)*  
[GitHub Profile](https://github.com/oangueram)
