# 🧮 Linear Equations Solver

> A command-line C++ application that parses, manipulates, and solves systems of linear equations using **Cramer's Rule** and **recursive cofactor expansion** — with zero external dependencies.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
  - [Running](#running)
- [Input Format](#input-format)
- [Supported Commands](#supported-commands)
- [Sample Session](#sample-session)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Limitations](#limitations)
- [Authors](#authors)

---

## Overview

This project was developed as a group assignment for solving linear equation systems via a text-based interface. The program reads a set of equations, stores them in flat runtime arrays, and responds to a series of commands — ranging from simple equation display to full system solving.

---

## ✨ Features

| Feature | Details |
|---|---|
| **Flexible parsing** | Handles implicit coefficients (`x` = `1x`, `-x` = `-1x`), signed terms, and constants |
| **10 commands** | Query, manipulate, and solve equations interactively |
| **Cramer's Rule** | Exact solution for square systems |
| **Determinant** | Recursive cofactor expansion |
| **No Solution detection** | Reports `No Solution` when determinant is zero |
| **Canonical output** | Variables always printed in sorted alphabetical order |
| **Pure C++** | No STL containers, no external libraries |

---

## 🚀 Getting Started

### Prerequisites

- A C++11-compatible compiler:
  - **Linux/macOS**: `g++` or `clang++`
  - **Windows**: MinGW, MSVC, or WSL

### Compilation

```bash
g++ -std=c++11 -o lin lin.cpp
```

Or with optimizations:

```bash
g++ -std=c++11 -O2 -o lin lin.cpp
```

### Running

**Interactive mode:**
```bash
./lin
```

**From a file:**
```bash
./lin < input.txt
```

**Windows:**
```cmd
lin.exe < input.txt
```

---

## 📄 Input Format

The first line is the number of equations `N`. Then `N` equations follow, one per line, in standard algebraic form.

**Rules:**
- Variable names are **lowercase letters only** (e.g. `x`, `y`, `ab`)
- Coefficients can be integers or decimals
- The `=` sign separates the left and right sides
- Constants on the left side are moved to the right automatically

**Examples of valid equations:**
```
2x+3y-z=10
-x+0.5y=4
abc-2def=0
x=5
```

After the equations, enter commands (one per line).

---

## 🛠️ Supported Commands

| Command | Description |
|---|---|
| `num_vars` | Print the number of unique variables across all equations |
| `equation <n>` | Print equation number `n` in canonical form (1-indexed) |
| `column <var>` | Print the coefficient of `var` in each equation |
| `add <i> <j>` | Print the result of adding equations `i` and `j` |
| `subtract <i> <j>` | Print the result of subtracting equation `j` from `i` |
| `substitute <var> <i> <j>` | Eliminate `var` from equation `i` using equation `j` |
| `D` | Print the coefficient matrix |
| `D <var>` | Print the Cramer matrix (column `var` replaced with RHS) |
| `D_value` | Print the determinant of the coefficient matrix |
| `solve` | Solve the system using Cramer's Rule |
| `quit` | Exit the program |

> ⚠️ Commands **do not modify** the stored equations. Results are printed temporarily.

---

## 💻 Sample Session

**Input (`input.txt`):**
```
2
2x+y=5
x-y=1
num_vars
equation 1
equation 2
column x
add 1 2
subtract 1 2
D_value
solve
quit
```

**Output:**
```
2
2x+y=5
-y+x=1
2
1
3x=6
x+2y=4
-3
x=2
y=1
```

---

## 📁 Project Structure

```
LinearEquationsSolver/
│
├── lin.cpp          # Main source file (all logic in one file)
├── README.md        # This file
└── input.txt        # Sample input (optional)
```

### Key Internal Components

| Component | Location | Description |
|---|---|---|
| Global arrays | Lines 7–11 | `varNames`, `varCoeffs`, `rightHandSide`, `varCount` |
| `parseLeftSide()` | ~Line 93 | Tokenizes equation LHS into variable/coefficient pairs |
| `collectAllVars()` | ~Line 130 | Gathers unique sorted variable names |
| `calcDeterminant()` | ~Line 145 | Recursive cofactor expansion |
| `buildCoeffMatrix()` | ~Line 165 | Builds matrix; supports Cramer column substitution |
| Main command loop | ~Line 200 | Reads and dispatches all 10 commands |

---

## ⚙️ How It Works

### 1. Parsing
Each equation string is tokenized at `+` and `-` boundaries. Each token is split at the first lowercase letter to separate the coefficient from the variable name. Constants (no letters) are moved to the RHS.

### 2. Storage
Variables are stored in flat 1D arrays indexed as `eqIndex * maxVars + varSlot`, avoiding the use of 2D arrays or STL containers.

### 3. Solving
Cramer's Rule is applied:
```
xₖ = det(Aₖ) / det(A)
```
Where `Aₖ` is the coefficient matrix with column `k` replaced by the RHS vector. If `det(A) = 0`, the system reports **No Solution**.

---

## ⚠️ Limitations

- Only handles **square systems** (same number of equations as variables) for `solve`
- No detection of **infinite solutions** (only "No Solution" is reported)
- `det(A) = 0` is treated as no solution, which may also indicate infinite solutions
- Floating-point precision may cause minor rounding in output
- Variable names must be **all lowercase letters** (digits and uppercase not supported)

---

## 👥 Authors

| Name | Student ID | Role |
|---|---|---|
| [Student 1] | [ID] | Team Leader – Integration, Parsing, Report |
| [Student 2] | [ID] | Data Structures, Memory Management |
| [Student 3] | [ID] | Command Loop, Arithmetic Operations |
| [Student 4] | [ID] | Matrix Functions, Cramer's Rule, Determinant |

---

## 📚 References

- Cormen et al., *Introduction to Algorithms* (3rd ed.) — MIT Press
- Stroustrup, *The C++ Programming Language* (4th ed.) — Addison-Wesley
- Anton & Rorres, *Elementary Linear Algebra* (11th ed.) — Wiley
- [Cramer's Rule – Wikipedia](https://en.wikipedia.org/wiki/Cramer%27s_rule)

---

<p align="center">
  Made as part of a C++ Programming course group project
</p>
