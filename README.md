# Connect-4 with Minimax AI (C)

An efficient, modular implementation of the classic **Connect-4** (4 en ratlla) game on an $N \times N$ board ($8 \times 8$), featuring an autonomous opponent powered by the **Minimax decision algorithm** with dynamic tree search and custom positional heuristics.

Developed as part of the *Advanced Programming* curriculum (Degree in Mathematics, Universitat Autònoma de Barcelona - UAB).

---

## Features

- **Arbitrary Depth Minimax Tree**: Dynamic n-ary game tree generated at runtime with rigorous dynamic memory management (`malloc` and post-order recursive `free`).
- **Multi-Factor Heuristic Evaluation**:
  - **Center-Column Weighting**: Rewards central positional control, maximizing line connection potentials.
  - **Window-Based Line Scanner**: Evaluates all 4-cell segments (horizontal, vertical, and both diagonals) to reward active 2-in-a-row / 3-in-a-row threats and penalize opponent threats.
  - **Depth-Discounted Terminal Scoring**: Prioritizes earlier wins over distant ones and seeks to prolong the game against inevitable losses.
- **Fair Move Selection & Tactical Blocking**: Uniform pseudo-random tie-breaking across tied optimal choices, coupled with an emergency scanner against imminent terminal threats.
- **Terminal UI**: Clean ANSI-colored board representation (Red for human `'X'`, Yellow for AI `'O'`).

---

## Project Structure

```text
├── 4enratlla.h       # Game engine prototypes, constants, and board definitions
├── 4enratlla.c       # Game loop, gravity drop logic, win/draw checks, terminal UI
├── minimax.h         # Game tree data structures and AI engine prototypes
├── minimax.c         # Tree generation, evaluation functions, and Minimax orchestration
├── Makefile          # Build automation script
└── .gitignore        # Git exclusion list for binaries and build artifacts
```

---

## Getting Started

### Prerequisites

You need a C compiler supporting the C99 standard, such as **GCC** (via MinGW-w64 on Windows or native GCC on Linux/macOS).

Check your installation:
```bash
gcc --version
```

### Compilation

You can build the executable using the provided `Makefile`:

```bash
# Using make (Linux / macOS / MinGW):
make

# Or on Windows with MinGW:
mingw32-make
```

Alternatively, compile directly with `gcc`:
```bash
gcc -Wall -Wextra -std=c99 -O2 4enratlla.c minimax.c -o 4enratlla.exe
```

### Running the Game

Launch the compiled executable:

```bash
# Windows
.\4enratlla.exe

# Linux / macOS
./4enratlla.exe
```

---

## Technical Details

- **Board Size**: Default is $8 \times 8$, defined by the compile-time constant `#define N 8` in `4enratlla.h`.
- **Search Depth**: The default lookahead depth is set to 4 half-moves (configurable via `PROFUNDITAT` in `4enratlla.c`).
- **Memory Safety**: Every tree allocation is guaranteed to be completely freed at the end of each AI turn, preventing memory leaks over extended play sessions.

---

## Author

- **Oriol Anguera** - *Degree in Mathematics, Universitat Autònoma de Barcelona (UAB)*
