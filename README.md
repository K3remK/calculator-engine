<p align="center">
  <h1 align="center">🧮 Calculator Engine</h1>
  <p align="center">
    <strong>A fast, extensible mathematical expression evaluator built from scratch in modern C++17</strong>
  </p>
  <p align="center">
    <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17">
    <img src="https://img.shields.io/badge/CMake-3.14+-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake">
    <img src="https://img.shields.io/badge/Tests-GoogleTest-4285F4?style=for-the-badge&logo=google&logoColor=white" alt="GoogleTest">
    <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" alt="License">
  </p>
</p>

---

## ✨ Overview

**Calculator Engine** is a modular mathematical expression evaluator that parses and computes human-readable infix expressions like `sqrt(3^2 + 4^2)` or `min(1, 2, 3, 4, 5)`. It follows a classic compiler pipeline — **Lexing → Parsing → Evaluation** — implementing the [Shunting-Yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) for correct operator precedence and associativity.

```
Input:  "sqrt(3^2 + 4^2)"
         │
    ┌────▼────┐
    │  Lexer  │   Tokenizes raw string into structured tokens
    └────┬────┘
         │  [3, ^, 2, +, 4, ^, 2] + function info
    ┌────▼────┐
    │ Parser  │   Converts infix → postfix (Shunting-Yard)
    └────┬────┘
         │  [3, 2, ^, 4, 2, ^, +, sqrt]
   ┌─────▼──────┐
   │ Evaluator  │   Stack-based postfix evaluation
   └─────┬──────┘
         │
Output:  5.0
```

---

## 🚀 Features

| Category | Supported |
|---|---|
| **Arithmetic** | `+`  `-`  `*`  `/`  `^` (power)  `mod` (modulus) |
| **Unary Operators** | `-` (negation)  `+` (identity) — e.g. `-3 + 1`, `10+---10`, `-sqrt(16)` |
| **Trigonometry** | `sin`  `cos`  `tan`  `cot` *(expects degrees)* |
| **Functions** | `sqrt`  `min`  `max` |
| **Postfix Operators** | `!` (factorial)  `%` (percent) — e.g. `5!`, `80%` |
| **Constants** | `pi` — Archimedes' constant (≈ 3.14159) |
| **Variadic** | `min(1,2,3,...,n)`  `max(1,2,3,...,n)` — any number of arguments |
| **Nesting** | Fully nested expressions: `sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1)))` |
| **Precedence** | Correct mathematical operator precedence and associativity |
| **Whitespace** | Flexible — spaces are optional: `2+3*4` and `2 + 3 * 4` both work |

---

## 🏗️ Architecture

The engine is split into **four cleanly separated modules**, each with a single responsibility:

### `Token` — Type System
Defines all supported token types using bitmask enums for efficient type checking. Contains operator metadata (precedence, associativity) and function dispatchers (`UnaryFunc`, `BinaryFunc`, `VariadicFunc`) via `std::variant`.

### `Lexer` — Tokenization
Scans the raw input string character-by-character and produces a flat sequence of `Token` objects. Handles multi-character identifiers (e.g., `sqrt`, `min`), floating-point number literals, and special symbols like `!` and `%`.

### `Parser` — Infix to Postfix Conversion
Implements the **Shunting-Yard algorithm** to convert infix token sequences into postfix (Reverse Polish Notation). Correctly handles:
- Operator precedence and left/right associativity
- Nested parenthesized expressions
- Function calls with variadic argument counting
- **Unary minus and plus detection** — automatically distinguishes between `-` as subtraction and `-` as negation based on context (e.g., `-3`, `5 * -2`, `--3`)

### `Evaluator` — Stack-Based Computation
Walks the postfix token sequence and evaluates it using a stack. Leverages `std::visit` on the operator's `std::variant` action to dynamically dispatch unary, binary, and variadic operations.

```
┌──────────────────────────────────────────────────────┐
│                     main.cpp                         │
│            (demo driver with sample expressions)     │
└───────────┬──────────────────────────────────────────┘
            │
   ┌────────▼────────┐
   │   Lexer.hpp/cpp │──→ Token.hpp (types + operators)
   └────────┬────────┘
            │
   ┌────────▼────────┐
   │  Parser.hpp/cpp │──→ Token.hpp
   └────────┬────────┘
            │
   ┌────────▼──────────┐
   │ Evaluator.hpp/cpp │──→ Token.hpp
   └───────────────────┘
```

---

## 📦 Prerequisites

- **C++17** compatible compiler (`g++ 7+`, `clang++ 5+`, or MSVC 2017+)
- **CMake** 3.14 or higher

---

## 🔧 Build & Run

```bash
# Clone the repository
git clone https://github.com/K3remK/calculator-engine.git
cd calculator-engine

# Configure and build
cmake -B build -S .
cmake --build build

# Run the calculator
./build/calculator-engine
```

### Example Output

```
cos(-pi)                                             -> π - cos  = -1
-3 + 1                                               -> 3 - 1 +  = -2
sin(-90)                                             -> 90 - sin  = -1
10+---10                                             -> 10 10 - - -  = 0
-sqrt(+min(+100, 200, 6, 29, max(5, 2, 4, 1)))      -> 100 + 200 6 29 5 2 4 1 max min + sqrt -  = -2.44948975
2 + 3 * 4                                            -> 2 3 4 * +  = 14
sqrt(3^2 + 4^2)                                      -> 3 2 ^ 4 2 ^ + sqrt  = 5
(80*20%)!                                            -> 80 20 % * !  = 20922789888000
100 mod 13                                           -> 100 13 mod  = 9
3*pi/2                                               -> 3 π * 2 /  = 4.71238898
```

---

## 🧪 Testing

The project includes a comprehensive test suite built with [Google Test](https://github.com/google/googletest), covering both **unit tests** and **integration tests**.

### Running Tests

```bash
# Build (GoogleTest is fetched automatically via CMake FetchContent)
cmake -B build -S .
cmake --build build

# Run all tests
cd build && ctest --output-on-failure

# Or run the test binary directly for detailed output
./build/unit_tests
```

### Test Structure

| Test File | Module | Tests | Description |
|---|---|---|---|
| `test_token.cpp` | Token | 34 | Token type classification, operator info, `toString()`, and all `operatorMap` lambdas |
| `test_lexer.cpp` | Lexer | 25 | Number/identifier tokenization, operators, whitespace, full expressions, error cases |
| `test_parser.cpp` | Parser | 18 | Precedence, parentheses, power associativity, unary minus, functions, postfix ops, errors |
| `test_evaluator.cpp` | Evaluator | 15 | Binary/unary/variadic ops, multi-step expressions, constants |
| `test_integration.cpp` | End-to-End | 20 | Full pipeline (`Lexer → Parser → Evaluator`) for arithmetic, trig, negation, nesting |

Tests are organized into logical sections within each file:
- **Unit tests** isolate each module (Token, Lexer, Parser, Evaluator) and test it independently
- **Integration tests** run the full pipeline from raw string input to final numeric result
- **Error cases** verify that malformed inputs throw appropriate exceptions

---

## 📁 Project Structure

```
calculator-engine/
├── CMakeLists.txt           # Build config (includes GoogleTest via FetchContent)
├── README.md                # This file
├── src/
│   ├── main.cpp             # Demo driver with sample expressions
│   ├── Token.hpp            # Token types, operator info & function dispatch
│   ├── Lexer.hpp            # Lexer class declaration + keyword map
│   ├── Lexer.cpp            # Tokenization implementation
│   ├── Parser.hpp           # Parser class declaration
│   ├── Parser.cpp           # Shunting-Yard algorithm implementation
│   ├── Evaluator.hpp        # Evaluator class declaration
│   ├── Evaluator.cpp        # Postfix evaluation implementation
│   └── Util.hpp             # Utility helpers
├── tests/
│   ├── test_main.cpp        # GoogleTest main entry point
│   ├── test_token.cpp       # Token type system unit tests
│   ├── test_lexer.cpp       # Lexer unit tests
│   ├── test_parser.cpp      # Parser (Shunting-Yard) unit tests
│   ├── test_evaluator.cpp   # Evaluator unit tests
│   └── test_integration.cpp # End-to-end integration tests
└── .gitignore               # Git ignore rules
```

---

## 🧪 How It Works — Step by Step

Let's trace `(2 + 3) * 4` through the engine:

**1. Lexing** — `Lexer::Tokenize("(2 + 3) * 4")`
```
Tokens: [ LeftParen, 2, Add, 3, RightParen, Mul, 4 ]
```

**2. Parsing** — `Parser::ToPostfix(tokens)` *(Shunting-Yard)*
```
Postfix: [ 2, 3, +, 4, * ]
```

**3. Evaluation** — `Evaluator::Evaluate(postfix)`
```
Stack trace:
  push 2       → [2]
  push 3       → [2, 3]
  apply +      → [5]         (2 + 3 = 5)
  push 4       → [5, 4]
  apply *      → [20]        (5 × 4 = 20)

Result: 20
```

### Unary Minus Example

Tracing `-3 + 1`:

**1. Lexing** — produces `[ Sub, 3, Add, 1 ]`
**2. Parsing** — the parser sees `-` at the start (no preceding number), so it re-classifies `Sub` → `UnaryMinus`:
```
Postfix: [ 3, UnaryMinus, 1, + ]
```
**3. Evaluation**
```
Stack trace:
  push 3       → [3]
  apply -      → [-3]        (negate 3)
  push 1       → [-3, 1]
  apply +      → [-2]        (-3 + 1 = -2)

Result: -2
```

---

## 🔑 Key Design Decisions

| Decision | Rationale |
|---|---|
| **Bitmask enum for `TokenType`** | Enables fast category checks via bitwise AND (e.g., `type & MathFunctions`) |
| **`std::variant` for operator actions** | Type-safe polymorphic dispatch without virtual functions or inheritance |
| **Shunting-Yard algorithm** | Industry-standard O(n) algorithm for infix-to-postfix conversion |
| **Degrees for trigonometry** | More intuitive for end-users (internal conversion: `value × π / 180`) |
| **Static `Tokenize` / `ToPostfix` / `Evaluate`** | Functional-style API — no need to manage object lifecycle |
| **Context-based unary detection** | Parser detects unary `-`/`+` when no operand precedes them, avoiding the need for a separate pre-processing pass |
| **GoogleTest via FetchContent** | Zero-config testing — CMake downloads GoogleTest automatically, no manual setup required |

---

## 🛣️ Roadmap

- [ ] Global exception handler for malformed expressions
- [x] Support for negative numbers and unary minus
- [x] Unary plus support
- [x] Modulus operator (`mod`)
- [x] Factorial operator (`!`)
- [x] Percent operator (`%`)
- [x] Pi constant (`pi`)
- [x] Unit and integration test suite (GoogleTest)
- [ ] Additional functions (`log`, `ln`, `abs`)
- [ ] Variable support (`x = 5; 2*x + 3`)
- [ ] Interactive REPL mode
- [ ] Multidimensional calculations

---

## 🤝 Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add my feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

---

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

---

<p align="center">
  <sub>Built with ❤️ and modern C++17</sub>
</p>
