
## Overview

This repository contains two programs written in C++:

- `asm.cpp` — a two-pass (single-routine) **Assembler** that converts `.asm` source files into object files.
- `emu.cpp` — an **Emulator** that executes the produced `.o` object files and provides an interactive trace/dump interface.

---

## Highlights / Features

### Assembler (asm.cpp)
- Uses two internal data structures: one for labels and one for the mnemonic table (with expected operands).
- Uses a single routine for both passes (symbol table build + code generation).
- Detects and reports all types of errors along with the line number.
- Stops after the first pass if any error is found.
- Produces three output files:
  - `.lst` — Listing file (source with addresses / resolved symbols).
  - `.o` — Object / binary file to be fed to the emulator.
  - `.log` — Detailed assembler log (errors, warnings, actions).
- Supports `SET` instructions (symbolic constants).
- Accepted input: a `.asm` file provided on the assembler command line.
- **Important:** All runtime/data inputs (if any) should be done at the *end* of the `.asm` program — this is a repository convention.

### Emulator (emu.cpp)
- Loads `.o` object files produced by the assembler.
- Maintains and reports mnemonic outputs along with register values, Program Counter (PC) and Stack Pointer (SP).
- Interactive options for execution:
  - `trace` (one step) — step through next instruction,
  - `all` — run to completion,
  - `dump` — memory dump by address and length,
  - `exit` — terminate emulator.
- `dump` flow: when you select `dump`, the emulator asks for:
  1. start address to dump,
  2. number of words/bytes to dump — and then prints those memory contents.
- All assembled programs have been tested via the emulator (including algorithmic test cases such as bubble sort).

---

## Requirements

- `g++` (GNU C++ compiler) — tested with typical Linux toolchains.
- Unix-like shell to run sample commands (Linux/macOS). Works on Windows if `g++` is available (MinGW / WSL).

---

## Build

Compile the assembler and emulator with:

```bash
g++ asm.cpp -o asm
g++ emu.cpp -o emu
```

Both files should compile without additional flags if the source conforms to the project specification. If headers or other files are required (e.g., additional `.cpp` / `.h` files), include them on the `g++` command line.

---

## Usage

### Assembler
```bash
# Assemble an input file `program.asm`
./asm program.asm
```
Outputs generated in the same directory:
- `program.lst`
- `program.o`
- `program.log`

If the assembler detects errors during pass 1 or pass 2 it will:
- stop at the first pass if a fatal error is found, print the line number and an informative message, and exit non-zero.
- write diagnostics to `program.log`.

**Notes**
- Ensure `program.asm` places any runtime data inputs at the end of the file (project convention).
- `SET` directives (symbol assignments) are supported — they are resolved during assembly.

### Emulator
```bash
# Run emulator on produced object file
./emu program.o
```

Once started, the emulator will present options (interactive):
- `trace` (or `t`) — execute one instruction and show mnemonic + register state.
- `all` — run program to termination and present final state.
- `dump` — memory dump:
  1. Enter start address (hex or decimal depending on implementation)
  2. Enter number of words/bytes to dump
  3. View printed dump
- `exit` — terminate emulator.

---

## Test Cases

This repository includes test cases covering:
- Bubble Sort (bubble_sort.asm)
- Additional algorithmic tests (sorting/searching/lookup examples)
- Small utilities that demonstrate `SET`, labels, stack usage, and memory operations

Run assembler + emulator for a test case:
```bash
./asm tests/bubble_sort.asm
./emu tests/bubble_sort.o
```

---

## Output File Formats (High-level)

- `.lst` — Human-readable source listing with addresses, resolved symbols and optionally machine code bytes per line.
- `.o` — Binary or text-based object file (implementation-specific). The emulator reads this format to reconstruct memory & instructions.
- `.log` — Plain-text log containing assembler messages, warnings and error diagnostics with line numbers.

> If you need the exact binary format described here (opcode encodings, endianness, header layout), I will update the README after reviewing `asm.cpp` / `emu.cpp` (or you can paste the exact description).

---

## Error Handling & Debugging

- The assembler prints line-numbered errors and writes details to the `.log`.
- The assembler halts on the first-pass fatal error (so you can fix definitions & labels before a second pass).
- Common issues:
  - Missing labels
  - Wrong number of operands for a mnemonic
  - Invalid `SET` values
  - Bad immediate or addressing modes
- Use `program.log` for a full audit trail of what the assembler did.

---

## Developer Notes & Conventions

- All source files contain a declaration of authorship and the author name + user id at the top of each file.
- Sensible identifiers (variable / function / type names) and explanatory comments are included per the project conventions.
- Data inputs (for test programs) are expected at the end of `.asm` files (assembler reads them as data sections).
- The assembler uses two tables:
  - Symbol table (labels & their addresses)
  - Mnemonic table (instruction -> expected operands & opcode)
- The assembler's single routine is re-used for both pass-one symbol collection and pass-two code generation; errors are reported with line numbers.

---

## Troubleshooting

- **Compilation fails** — check you have `g++` installed and you are in the source directory. If missing headers or multiple source files exist, compile all of them together:
  ```bash
  g++ asm.cpp helper1.cpp -o asm
  g++ emu.cpp helper2.cpp -o emu
  ```
- **Assembler crashes** — examine `program.log` and the `.lst` file for the last processed line and the error message. Fix labels/SET directives and try again.
- **Emulator behavior unexpected** — ensure the `.o` file was produced by the `asm` built from the same source tree and not modified.

---

