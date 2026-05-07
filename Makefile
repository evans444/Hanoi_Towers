# ─────────────────────────────────────────────────────────────────
# Makefile — Towers of Hanoi
# Targets:
#   make          →  Build CLI-only version
#   make gtk      →  Build CLI + GTK animated window version
#   make run      →  Build & run CLI version (default 3 disks)
#   make run-gtk  →  Build & run GTK version
#   make clean    →  Remove compiled binaries
# ─────────────────────────────────────────────────────────────────

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2
TARGET  = hanoi
GTK_TARGET = hanoi_gtk

# GTK flags (via pkg-config)
GTK_CFLAGS  := $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS    := $(shell pkg-config --libs   gtk+-3.0 2>/dev/null)

DISKS ?= 3

.PHONY: all gtk run run-gtk clean help

# ── Default: CLI-only build ───────────────────────────────────────
all: $(TARGET)

$(TARGET): hanoi.c
	$(CC) $(CFLAGS) -o $(TARGET) hanoi.c
	@echo "  ✓  Built '$(TARGET)' (CLI only)"

# ── GTK build ────────────────────────────────────────────────────
gtk: $(GTK_TARGET)

$(GTK_TARGET): hanoi.c
	@if [ -z "$(GTK_CFLAGS)" ]; then \
	    echo "ERROR: gtk+-3.0 not found. Install with:"; \
	    echo "  Ubuntu/Debian : sudo apt install libgtk-3-dev"; \
	    echo "  Fedora        : sudo dnf install gtk3-devel"; \
	    echo "  Arch          : sudo pacman -S gtk3"; \
	    exit 1; \
	fi
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -DUSE_GTK -o $(GTK_TARGET) hanoi.c $(GTK_LIBS)
	@echo "  ✓  Built '$(GTK_TARGET)' (CLI + GTK animated window)"

# ── Run targets ──────────────────────────────────────────────────
run: $(TARGET)
	./$(TARGET) $(DISKS)

run-gtk: $(GTK_TARGET)
	./$(GTK_TARGET) $(DISKS)

# ── Cleanup ──────────────────────────────────────────────────────
clean:
	rm -f $(TARGET) $(GTK_TARGET)
	@echo "  ✓  Cleaned build artefacts"

# ── Help ─────────────────────────────────────────────────────────
help:
	@echo ""
	@echo "  Towers of Hanoi — Build targets"
	@echo "  ────────────────────────────────────"
	@echo "  make              Build CLI version"
	@echo "  make gtk          Build CLI + GTK version"
	@echo "  make run          Build & run CLI  (DISKS=n to change disk count)"
	@echo "  make run-gtk      Build & run GTK  (DISKS=n to change disk count)"
	@echo "  make clean        Remove binaries"
	@echo ""
	@echo "  Examples:"
	@echo "    make run DISKS=4"
	@echo "    make run-gtk DISKS=5"
	@echo ""
