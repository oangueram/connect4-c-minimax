# Connect-4 with Minimax, Alpha-Beta Pruning & Transposition Tables (C)

An ultra-fast, modular implementation of the classic **Connect-4** (4 en ratlla) game on an $N \times N$ board ($8 \times 8$), featuring a high-performance AI opponent powered by the **Minimax algorithm with Alpha-Beta pruning**, center-out **Move Ordering**, **64-bit Zobrist Hashing Transposition Tables**, and tactical **Gravity-Aware Heuristics**.

Developed for the *Programació Avançada* course (Degree in Mathematics, Universitat Autònoma de Barcelona - UAB).

---

## Key Features & Optimizations

### 1. Alpha-Beta Pruning with Strict Inequalities
- Implements minimax tree search with $[\alpha, \beta]$ window bounds.
- Strict cutoffs (`alpha > beta` on MAX turns and `beta < alpha` on MIN turns) prevent edge-case ties while maximizing branch pruning.
- Reduces theoretical search complexity from $O(b^d)$ towards $O(b^{d/2})$ under optimal move ordering.

### 2. On-the-Fly Dynamic Memory Management
- Replaces static upfront tree allocation with dynamic node generation on demand.
- Nodes are allocated, recursively evaluated, and immediately deallocated (`malloc` paired with immediate `free`).
- Peak memory consumption is reduced from **hundreds of megabytes** to a minimal memory footprint proportional only to the maximum search depth, strictly maintaining $\text{mallocs} == \text{frees}$.

### 3. Center-Out Move Ordering
- Child nodes are explored prioritizing columns from the center outwards:
  $$\text{Exploration Order: } [3, 4, 2, 5, 1, 6, 0, 7]$$
- Because central columns are inherently more valuable, Alpha-Beta finds high $\alpha$ and low $\beta$ bounds immediately, **pruning up to 90% of sub-branches** and speeding up execution by nearly an order of magnitude.

### 4. Transposition Tables with 64-bit Zobrist Hashing
- **$O(1)$ Incremental State Hashing**: Utilizes 64-bit pseudo-random numbers per board coordinate and piece type (`taulaZobrist[N][N][2]`). Board states update in $O(1)$ time upon placing a token using bitwise XOR ($\oplus$):
  $$\text{hash}_{\text{child}} = \text{hash}_{\text{parent}} \oplus \text{Zobrist}[\text{row}][\text{col}][\text{piece}]$$
- **High-Capacity Hash Table**: Direct-mapped table with $2^{20} = 1,048,576$ entries (`MIDA_TAULA`), leveraging fast bitwise indexing (`hash % MIDA_TAULA`).
- **Mathematical Bound Classification**: Preserves exact values vs pruned cuts:
  - `TIPUS_EXACTE` (0): True minimax value without cutoff.
  - `TIPUS_COTA_INFERIOR` (1): Beta cutoff ($\ge \beta_{\text{original}}$), providing a rigorous lower bound.
  - `TIPUS_COTA_SUPERIOR` (2): Alpha cutoff or fail-low ($\le \alpha_{\text{original}}$), providing a rigorous upper bound.
- **Depth-Preferred Replacement Policy**: Replaces cache entries when $\text{depth}_{\text{remaining}} \ge \text{depth}_{\text{cached}}$, protecting deep, high-computation sub-trees while refreshing obsolete states from prior turns.

### 5. Multi-Factor Heuristic with Gravity-Aware Threat Modeling
- **Center-Column Weighting (`avaluarCentre`)**: Rewards central column occupation to maximize future connection angles.
- **Window Scanner**: Scans all 4-cell segments across horizontal, vertical, and diagonal orientations.
- **Gravity & Support Verification (`teSuport`)**: Distinguishes between immediate executable threats (supported directly from below) versus floating future threats/traps.
- **Double-Threat (*Fork*) Bonus**: Detects states with multiple simultaneous 3-in-a-row threats ($\ge 2$), rewarding unstoppable forced-win paths with a $+5000$ heuristic bonus.
- **Depth-Discounted Terminal Values**: Wins are scored as $1,000,000 - \text{depth} \times 10,000$, forcing the AI to execute the fastest possible checkmate and delay opponent victories.

---

## Performance Benchmarks

Measured on an Intel Core i7 system ($8 \times 8$ board, opening game phase):

| Depth | Plies (AI / Human) | Time (Without Move Ordering) | Time (Move Ordering) | Time (TT + Move Ordering) | Overall Speedup |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **2** | 1 / 1 | < 0.001 s | < 0.001 s | < 0.001 s | Instant |
| **4** | 2 / 2 | 0.015 s | 0.003 s | 0.001 s | **$15\times$** |
| **6** | 3 / 3 | 0.220 s | 0.018 s | 0.005 s | **$44\times$** |
| **8** | 4 / 4 | 3.580 s | 0.090 s | **0.022 s** | **$160\times$** |
| **10** | 5 / 5 | > 60 s | 1.410 s | **0.280 s** | **Real-time** |

---

## Project Structure

```text
├── 4enratlla.h       # Board constants (N=8) and game engine prototypes
├── 4enratlla.c       # Main game loop, gravity drop logic, win checks, terminal UI
├── minimax.h         # Tree node structures, TT definitions and Minimax prototypes
├── minimax.c         # Alpha-Beta search, dynamic memory, Zobrist TT, heuristic
├── Makefile          # Build automation with clean and compiler flags
└── README.md         # Project documentation, algorithms and benchmarks
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
