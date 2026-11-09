# Connect-4 with Minimax & Alpha-Beta Pruning (C)

An ultra-fast, modular implementation of the classic **Connect-4** (4 en ratlla) game on an $N \times N$ board ($8 \times 8$), featuring an AI opponent powered by the **Minimax algorithm with Alpha-Beta pruning**, center-out **Move Ordering**, and tactical **Double-Threat (*Fork*) heuristics**.

Developed for the *Programació Avançada* course (Degree in Mathematics, Universitat Autònoma de Barcelona - UAB).

---

## Key Features & Optimizations

### 1. Alpha-Beta Pruning with Strict Inequalities
- Implements minimax tree search with $(\alpha, \beta)$ window bounds.
- Strict cutoffs (`alpha > beta` on MAX turns and `beta < alpha` on MIN turns) prevent edge-case ties while maximizing branch pruning.

### 2. On-the-Fly Dynamic Memory Management
- Replaces static upfront tree allocation with dynamic node generation on demand.
- Nodes are allocated, recursively evaluated, and immediately deallocated (`malloc` paired with immediate `free`).
- Peak memory consumption is reduced from **hundreds of megabytes** to a minimal memory footprint proportional only to the maximum search depth.

### 3. Center-Out Move Ordering
- Child nodes are explored prioritizing columns from the center outwards:
  $$\text{Exploration Order: } [3, 4, 2, 5, 1, 6, 0, 7]$$
- Because central columns are inherently more valuable, Alpha-Beta finds high $\alpha$ and low $\beta$ bounds immediately, **pruning up to 90% of sub-branches** and speeding up execution by nearly an order of magnitude.

### 4. Advanced Multi-Factor Heuristic
- **Center-Column Weighting**: Rewards central column occupation to maximize future connection angles.
- **Window Scanner**: Scans all 4-cell segments across horizontal, vertical, and diagonal orientations.
- **Exponential Threat Scoring**: High-threat 3-in-a-row lines are prioritized over passive positional consolidation.
- **Double-Threat (*Fork*) Bonus**: Detects states with multiple simultaneous 3-in-a-row threats ($\ge 2$), rewarding unstoppable forced-win paths with a $+5000$ heuristic bonus.
- **Depth-Discounted Terminal Values**: Wins are scored as `1,000,000 - depth`, forcing the AI to execute the fastest possible checkmate.

---

## Performance Benchmarks

Measured on an Intel Core i7 system ($8 \times 8$ board, opening game phase):

| Depth | Plies (AI / Human) | Time (Without Move Ordering) | Time (With Move Ordering) | Speedup |
| :---: | :---: | :---: | :---: | :---: |
| **2** | 1 / 1 | < 0.001 s | < 0.001 s | Instant |
| **4** | 2 / 2 | 0.015 s | 0.003 s | $5\times$ |
| **6** | 3 / 3 | 0.220 s | 0.018 s | $12\times$ |
| **8** | 4 / 4 | 3.580 s | **0.090 s** | **$40\times$** |
| **10** | 5 / 5 | > 60 s | **1.410 s** | **Real-time** |

---

## Project Structure

```text
├── 4enratlla.h       # Board constants (N=8) and game engine prototypes
├── 4enratlla.c       # Main game loop, gravity drop logic, win checks, terminal UI
├── minimax.h         # Tree node structures and Minimax function declarations
├── minimax.c         # Alpha-Beta search, dynamic memory, Move Ordering, heuristic
├── Makefile          # Build automation with header dependency tracking
└── README.md         # Project documentation and benchmarks
```

---

## Getting Started

### Prerequisites

A C compiler supporting **C99**, such as **GCC** (via MinGW-w64 on Windows or native GCC on Linux/macOS).

Check your installation:
```bash
gcc --version
```

### Compilation

Build the executable using `make`:

```bash
# Windows (MinGW):
mingw32-make

# Linux / macOS:
make
```

Or compile directly with optimization flags:
```bash
gcc -Wall -Wextra -std=c99 -O2 4enratlla.c minimax.c -o 4enratlla.exe
```

### Running the Game

```bash
# Windows:
.\4enratlla.exe

# Linux / macOS:
./4enratlla.exe
```

---

## Author

- **Oriol Anguera** - *Degree in Mathematics, Universitat Autònoma de Barcelona (UAB)*
