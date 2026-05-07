================================================================
  TOWERS OF HANOI — C Programming Assignment
  Chapter 5, Exercise 5.39  (C Functions)
================================================================

PROBLEM DESCRIPTION
-------------------
The Towers of Hanoi is a classic recursive puzzle.
You have three pegs and a stack of N disks of decreasing size
placed on peg 1. The goal is to move the entire stack to peg 3
following two rules:

  1. You may move only ONE disk at a time.
  2. A larger disk may NEVER be placed on top of a smaller disk.

A third peg is used as a temporary holding area.


HOW THE RECURSIVE ALGORITHM WORKS
----------------------------------
The key insight is to reduce the problem of moving N disks into
three smaller sub-problems:

  Step A: Move the top (N-1) disks from peg SRC to peg TMP,
          using peg DST as the temporary area.

  Step B: Move the one remaining (largest) disk from SRC to DST.

  Step C: Move the (N-1) disks from peg TMP to peg DST,
          using peg SRC as the temporary area.

Base case: When N = 0, do nothing (the recursion unwinds).

This gives an exact move count of  2^N - 1.
For 3 disks that is 7 moves; for 64 disks it would be
18,446,744,073,709,551,615 moves (why the legend says the world
will end when the priests finish!).


RECURSIVE FUNCTION SIGNATURE
------------------------------
  void hanoi(int n, int src, int dst, int tmp)

  Parameters:
    n   — number of disks to move in this call
    src — source peg      (0-indexed internally, displayed as 1-3)
    dst — destination peg
    tmp — temporary peg used for intermediate moves


PROGRAM FEATURES
-----------------
1. CLI ASCII Diagram
   - After EVERY move the program prints all three pegs with their
     current disks drawn as '#' characters.
   - Wider '#' rows represent larger disks.
   - Peg poles are drawn as '|'; the base as '===='.
   - A 0.2 s delay between moves lets you watch the state change.

2. GTK Animated Window  (built with 'make gtk')
   - Opens a dark-themed window showing the three pegs with
     coloured, rounded disk rectangles.
   - Controls:
       ▶ Play / ⏸ Pause  — auto-advance through all moves
       ◀ Prev / Next ▶   — step one move at a time
       ⟳ Reset           — return to the initial state
       Speed slider       — 0.5×  to  5× play speed
   - Disk numbers are printed inside each disk shape.
   - The current move (e.g. "Peg 1 → Peg 3") is shown above
     the drawing area.


FILE STRUCTURE
---------------
  hanoi/
  ├── hanoi.c      Main source file (CLI + optional GTK)
  ├── Makefile     Build system
  └── README.txt   This file


BUILD INSTRUCTIONS
-------------------
Prerequisites:
  - GCC (or any C11-compliant compiler)
  - make
  - For GTK build: libgtk-3-dev

  Ubuntu / Debian:   sudo apt install build-essential libgtk-3-dev
  Fedora:            sudo dnf install gcc make gtk3-devel
  Arch Linux:        sudo pacman -S base-devel gtk3
  macOS (Homebrew):  brew install gtk+3

Building:

  # CLI-only (no GTK needed)
  make

  # CLI + GTK animated window
  make gtk

  # Specify a custom number of disks (1–8)
  make run       DISKS=4
  make run-gtk   DISKS=5

  # Remove compiled files
  make clean


RUNNING THE PROGRAM
--------------------
  ./hanoi           # 3 disks, CLI only
  ./hanoi 4         # 4 disks, CLI only
  ./hanoi_gtk       # 3 disks, CLI + GTK window
  ./hanoi_gtk 5     # 5 disks, CLI + GTK window


EXAMPLE OUTPUT (3 disks)
--------------------------
  Move 1:  Peg 1 --> Peg 3

      |            |            |
      |            |            |
   #######         |            #
  #########        |         #######
  =========   =========   =========
      1            2            3

  Move 2:  Peg 1 --> Peg 2
  ... (7 moves total)

  Total moves : 7
  Formula     : 2^3 - 1 = 7


ALGORITHM COMPLEXITY
---------------------
  Time  : O(2^n)  — number of moves grows exponentially
  Space : O(n)    — maximum recursion depth equals n


UPLOADING TO GITHUB
--------------------
  git init
  git add hanoi.c Makefile README.txt
  git commit -m "Add Towers of Hanoi solution with CLI diagrams and GTK animation"
  git remote add origin https://github.com/<your-username>/<your-repo>.git
  git push -u origin main


AUTHOR NOTE
------------
This solution fulfils all requirements of Exercise 5.39:
  ✓ Recursive function with the four required parameters
  ✓ Prints the precise sequence of disk transfers  (e.g. 1 → 3)
  ✓ Step-by-step diagram visualisation on the CLI
  ✓ GTK animated window for enhanced visualisation

================================================================
