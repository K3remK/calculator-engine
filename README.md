<p align="center">
  <h1 align="center">🧮 Calculator Engine</h1>
  <p align="center">
    <strong>A fast, extensible mathematical expression evaluator built from scratch in modern C++17</strong>
  </p>
  <p align="center">
    <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17">
    <img src="https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake">
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
| **Arithmetic** | `+`  `-`  `*`  `/`  `^` (power) |
| **Trigonometry** | `sin`  `cos`  `tan`  `cot` *(expects degrees)* |
| **Functions** | `sqrt`  `min`  `max` |
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
Scans the raw input string character-by-character and produces a flat sequence of `Token` objects. Handles multi-character identifiers (e.g., `sqrt`, `min`) and floating-point number literals.

### `Parser` — Infix to Postfix Conversion
Implements the **Shunting-Yard algorithm** to convert infix token sequences into postfix (Reverse Polish Notation). Correctly handles:
- Operator precedence and left/right associativity
- Nested parenthesized expressions
- Function calls with variadic argument counting

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
- **CMake** 3.10 or higher

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
2^2^(2*sin(20))    -> 2 2 2 20 sin * ^ ^  = 2.69734286
min(1, 2, 3, 4, 5) -> 1 2 3 4 5 min  = 1
sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1))) -> 100 200 6 29 5 2 4 1 max min sqrt  = 2.44948975
sin(max(2,3)/3*3)  -> 2 3 max 3 / 3 * sin  = 0.0523359562
2 + 3 * 4          -> 2 3 4 * +  = 14
(2 + 3) * 4        -> 2 3 + 4 *  = 20
sqrt(3^2 + 4^2)    -> 3 2 ^ 4 2 ^ + sqrt  = 5
```

---

## 📁 Project Structure

```
calculator-engine/
├── CMakeLists.txt        # Build configuration
├── main.cpp              # Demo driver with sample expressions
├── Token.hpp             # Token types, operator info & function dispatch
├── Lexer.hpp             # Lexer class declaration
├── Lexer.cpp             # Tokenization implementation
├── Parser.hpp            # Parser class declaration
├── Parser.cpp            # Shunting-Yard algorithm implementation
├── Evaluator.hpp         # Evaluator class declaration
├── Evaluator.cpp         # Postfix evaluation implementation
├── Util.hpp              # Utility helpers
└── .gitignore            # Git ignore rules
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

---

## 🔑 Key Design Decisions

| Decision | Rationale |
|---|---|
| **Bitmask enum for `TokenType`** | Enables fast category checks via bitwise AND (e.g., `type & MathFunctions`) |
| **`std::variant` for operator actions** | Type-safe polymorphic dispatch without virtual functions or inheritance |
| **Shunting-Yard algorithm** | Industry-standard O(n) algorithm for infix-to-postfix conversion |
| **Degrees for trigonometry** | More intuitive for end-users (internal conversion: `value × π / 180`) |
| **Static `Tokenize` / `ToPostfix` / `Evaluate`** | Functional-style API — no need to manage object lifecycle |

---

## 🛣️ Roadmap

- [ ] Global exception handler for malformed expressions
- [x] Support for negative numbers and unary minus
- [ ] Additional functions (`log`, `ln`, `abs`, `factorial`)
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
