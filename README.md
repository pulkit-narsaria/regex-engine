# Regex‑Engine

A minimal regular‑expression engine implemented in C++20, built with Visual Studio 2022 for educational and demonstration purposes.

---

## 🚀 Table of Contents

* [Introduction](#introduction)
* [Features](#features)
* [Getting Started](#getting-started)
* [Architecture & Design](#architecture--design)
* [How It Works](#how-it-works)
* [Usage](#usage)
* [Contributing](#contributing)
  
---

## Introduction

This project implements a basic regex engine from scratch in C++20 using Visual Studio 2022. It showcases the parsing, compilation, and matching phases of regex processing and is ideal for learning core algorithms behind pattern matching.

---

## Features

* **Regex syntax**: supports `|` (union), concatenation, Kleene star `*`, character classes, and literal characters
* **Parser**: recursive‑descent or shunting‑yard based parser converting input expressions into internal representation
* **Automata‑based matching**: builds a Non‑Deterministic Finite Automaton (NFA) via Thompson’s construction
* **Matcher**: simulates NFA (with ε‑closures and state transitions) to decide match/no‑match
* **C++20 modern style**: using standard containers, smart pointers, `std::string_view`, etc.
* **Simple CLI**: interactive or command‑line input for pattern and text matching

---

## Getting Started

### Prerequisites

* Visual Studio 2022 with the Desktop C++ workload
* C++20 standard library support enabled

### Build instructions

1. Clone the repo:

   ```bash
   git clone https://github.com/pulkit-narsaria/regex-engine.git
   cd regex-engine
   ```
2. Open the `.sln` solution in Visual Studio 2022
3. Ensure project is set to **C++20** in Project → Properties → C/C++ → Language → C++ Language Standard
4. Build the project in Debug or Release mode
5. Run the resulting executable (via IDE or `.exe`)

---

## Architecture & Design

### Major components

* **Parser**
  Reads regex pattern string and produces post‑fix tokens representing operators and operands.

* **Compiler / Automata Builder**
  Constructs an NFA using Thompson’s algorithm: union, concatenation, closure, etc.

* **NFA Engine**
  Simulates the NFA: computes ε‑closures, then for each input character performs state transitions, tracking acceptance.

* **Matcher Interface**
  Implements match logic; client code constructs, compiles, and queries matches.

---

## How It Works

1. **Parsing**

   * The engine reads the input regex (e.g. `"a(b|c)*d"`) and builds a parse structure: literal nodes, alternation, grouping, closure.

2. **Thompson construction**

   * Each regex construct is translated into an NFA fragment:

     * **Union (`|`)**: introduces a new start and accept state, with ε‑edges to each branch
     * **Concatenation**: connects accept of one fragment to start of the next
     * **Kleene star (`*`)**: wraps fragment with loopable ε‑transitions
     * **Literal / character class**: direct transition via input character or set

3. **Simulation / Matching**

   * Compute ε‑closure of the initial state
   * For each input character:

     * Move from current states via matching transitions
     * Compute ε‑closure of result
   * After processing all characters, if any current state is accepting → match succeeds

4. **Complexity**

   * Matching runs in time proportional to (pattern size) × (input length), typical for backtracking-free NFA methods.

---

## Usage

Example of a simple CLI interface:

```
Enter regex> (a|b)*abb
Enter input string> aababb
Match found
```

---

## Contributing

Contributions welcome! Feel free to:

* File issues or feature requests via GitHub
* Submit pull requests to add features, improve parsing, or enhance performance
* Follow code style and include tests if adding functionality
