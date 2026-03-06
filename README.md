<p align="center">
  <h1 align="center">🧮 Calculator Engine</h1>
  <p align="center">
    <strong>A fast, extensible mathematical expression evaluator with matrix support — built from scratch in modern C++17</strong>
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

**Calculator Engine** is a modular mathematical expression evaluator that parses and computes human-readable infix expressions like `sqrt(3^2 + 4^2)`, `min(1, 2, 3, 4, 5)`, and even **matrix expressions** like `[1 2; 3 4] * [5; 6]`. It follows a classic compiler pipeline — **Lexing → Parsing → Evaluation** — implementing the [Shunting-Yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) for correct operator precedence and associativity. Results can be **scalars or matrices**, and the engine pretty-prints matrix outputs with vertically-centered alignment.

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

| Category              | Supported                                                                                                                                                          |
| --------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Arithmetic**        | `+` `-` `*` `/` `^` (power) `mod` (modulus)                                                                                                                        |
| **Unary Operators**   | `-` (negation) `+` (identity) — e.g. `-3 + 1`, `10+---10`, `-sqrt(16)`                                                                                             |
| **Trigonometry**      | `sin` `cos` `tan` `cot` _(expects degrees)_                                                                                                                        |
| **Functions**         | `sqrt` `min` `max` `abs` `log` (base 10) `ln` (natural) `logbase(value, base)`                                                                                     |
| **Postfix Operators** | `!` (factorial) `%` (percent) — e.g. `5!`, `80%`                                                                                                                   |
| **Constants**         | `pi` (≈ 3.14159) `e` — Euler's number (≈ 2.71828)                                                                                                                  |
| **Matrix Literals**   | Inline syntax: `[1 2 3; 4 5 6]` — rows separated by `;`, columns by spaces or commas. Each entry can be a full expression (e.g. `[2^2 cos(0); sin(0)+1 sqrt(16)]`) |
| **Matrix Arithmetic** | `+` `-` `*` `/` `^` between matrices and/or scalars (mixed-type expressions)                                                                                       |
| **Matrix Operations** | Transpose, Identity, Determinant, Inverse, element-wise `abs`, and integer power (`A^n`)                                                                           |
| **Linear Solve**      | `A \ b` — solve `Ax = b` via LU decomposition with partial pivoting and verification                                                                               |
| **Pretty Printing**   | Matrix results are displayed with bracket notation and vertically-centered alignment                                                                               |
| **Variadic**          | `min(1,2,3,...,n)` `max(1,2,3,...,n)` — any number of arguments                                                                                                    |
| **Nesting**           | Fully nested expressions: `sqrt(min(100, 200, 6, 29, max(5, 2, 4, 1)))`                                                                                            |
| **Precedence**        | Correct mathematical operator precedence and associativity                                                                                                         |
| **Whitespace**        | Flexible — spaces are optional: `2+3*4` and `2 + 3 * 4` both work                                                                                                  |

---

## 🏗️ Architecture

The engine is split into **six cleanly separated modules**, each with a single responsibility:

### `Token` — Type System

Defines all supported token types using bitmask enums for efficient type checking. Contains operator metadata (precedence, associativity) and the polymorphic `Value` type (`std::variant<double, Matrix<double>>`) that enables the engine to seamlessly operate on both scalars and matrices.

### `Matrix` — Matrix Data Structure

A header-only template class `Matrix<T>` providing:

- **Arithmetic**: matrix-matrix (`+`, `-`, `*`) and matrix-scalar (`+`, `-`, `*`, `/`) operations
- **Utilities**: `Transpose()`, `Identity()`, `IsSquare()`, `IsEmpty()`, element-wise `abs()`
- **Determinant**: Gaussian elimination with partial pivoting
- **Inverse**: Gauss-Jordan elimination on augmented `[A | I]` matrix
- **Solve**: `Matrix::Solve(A, b)` — LU decomposition with partial pivoting, forward/back substitution, and solution verification
- **Power**: integer matrix exponentiation via repeated multiplication
- **Stream output**: `operator<<` for formatted display

### `Lexer` — Tokenization

Scans the raw input string character-by-character and produces a flat sequence of `Token` objects. Handles multi-character identifiers (e.g., `sqrt`, `min`), floating-point number literals, special symbols like `!` and `%`, and **matrix literals** enclosed in `[ ]` with `;` as the row separator. Each matrix entry is recursively tokenized, parsed, and evaluated as an independent expression — so entries like `2^2`, `cos(0)`, or `sqrt(16)/5*5` are fully supported.

### `Parser` — Infix to Postfix Conversion

Implements the **Shunting-Yard algorithm** to convert infix token sequences into postfix (Reverse Polish Notation). Correctly handles:

- Operator precedence and left/right associativity
- Nested parenthesized expressions
- Function calls with variadic argument counting
- **Unary minus and plus detection** — automatically distinguishes between `-` as subtraction and `-` as negation based on context (e.g., `-3`, `5 * -2`, `--3`)
- **Matrix tokens** — matrix literals pass through as operands alongside numbers
- **Backslash operator** — `\` is parsed as the `InvMul` operator for `A \ b` linear system notation

### `Evaluator` — Stack-Based Computation

Walks the postfix token sequence and evaluates it using a stack. Leverages `std::visit` with the `overloaded` pattern to dynamically dispatch unary, binary, and variadic operations. Correctly handles mixed-type operations (e.g., `Matrix * scalar`, `scalar + Matrix`) through `std::variant` visitor dispatch. The `InvMul` (`\`) operator calls `Matrix::Solve(A, b)` for linear system solving.

### `PrettyPrint` — Output Rendering

Renders expression results with vertically-centered alignment. Matrices are displayed with bracket notation (`[ ... ]`), and all elements in a multi-line equation are vertically aligned to the tallest operand.

```
┌──────────────────────────────────────────────────────┐
│                     main.cpp                         │
│            (demo driver with sample expressions)     │
└───────────┬──────────────────────────────────────────┘
            │
   ┌────────▼────────┐
   │   Lexer.hpp/cpp │──→ Token.hpp ──→ Matrix.hpp
   └────────┬────────┘
            │
   ┌────────▼────────┐
   │  Parser.hpp/cpp │──→ Token.hpp
   └────────┬────────┘
            │
   ┌────────▼──────────┐
   │ Evaluator.hpp/cpp │──→ Token.hpp, Matrix.hpp
   └────────┬──────────┘
            │
   ┌────────▼───────────────┐
   │ PrettyPrint.hpp/cpp    │──→ Token.hpp, Matrix.hpp
   └────────────────────────┘
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

Scalar expressions:

```
2 + 3 * 4       = 14
sqrt(3^2 + 4^2) = 5
cos(-pi)        = -1
-3 + 1          = -2
(80*20%)!       = 20922789888000
100 mod 13      = 9
log(100)        = 2
logbase(8, 2)   = 3
abs(-5)         = 5
```

Matrix expressions:

```
                 [ 1 ]
[ 1  2  3  4 ]   [ 2 ]   [ 30 ]
[ 1  2  3  4 ] * [ 3 ] = [ 30 ]
                 [ 4 ]

   [ 1  2  3 ]   [ 1  2  3 ]             [ 0.6  1.2  1.8 ]
 ( [ 1  2  3 ] * [ 1  2  3 ] )  /  10  = [ 0.6  1.2  1.8 ]
   [ 1  2  3 ]   [ 1  2  3 ]             [ 0.6  1.2  1.8 ]

[ 1  2  3 ]               [ inf  inf  inf ]
[ 1  2  3 ] ^  1000000  = [ inf  inf  inf ]
[ 1  2  3 ]               [ inf  inf  inf ]
```

Matrix entries as expressions — each cell is evaluated independently:

```
[2^2  cos(0)  sin(0); cos(0)  2^2  cos(0); sin(0)  cos(0)  2^2] \ [6; 12; 14]

→  [ 4  1  0 ]       [ 6  ]   [ 1 ]
   [ 1  4  1 ]  \  [ 12 ] = [ 2 ]
   [ 0  1  4 ]       [ 14 ]   [ 3 ]
```

Linear system solving (`A \ b`):

```
[ 2  1 ]        [ 8  ]   [ 3 ]
[ 5  3 ]  \  [  21 ] = [ 2 ]
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

| Test File              | Module     | Description                                                                                                                    |
| ---------------------- | ---------- | ------------------------------------------------------------------------------------------------------------------------------ |
| `test_token.cpp`       | Token      | Token type classification, operator info, `toString()`, all `operatorMap` lambdas including log/ln/logbase/abs, and edge cases |
| `test_lexer.cpp`       | Lexer      | Number/identifier tokenization, operators, whitespace, matrix literals, `\` operator, malformed matrix errors                  |
| `test_parser.cpp`      | Parser     | Precedence, parentheses, power associativity, unary minus, functions, matrix postfix conversion, InvMul operator               |
| `test_evaluator.cpp`   | Evaluator  | Binary/unary/variadic ops, matrix arithmetic, matrix power, InvMul evaluation, argc validation, error cases                    |
| `test_integration.cpp` | End-to-End | Full pipeline for arithmetic, trig, negation, nesting, matrix operations, linear system solving, matrix error cases            |
| `test_matrix.cpp`      | Matrix     | Matrix constructors, arithmetic, scalar ops, Transpose, Identity, Determinant, Inverse, Solve, edge cases, malformed input     |

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
│   ├── Matrix.hpp           # Matrix<T> template class (arithmetic, transpose, determinant, inverse, solve)
│   ├── Matrix.cpp           # Matrix implementation helpers
│   ├── Lexer.hpp            # Lexer class declaration + keyword map
│   ├── Lexer.cpp            # Tokenization implementation (incl. matrix literals)
│   ├── Parser.hpp           # Parser class declaration
│   ├── Parser.cpp           # Shunting-Yard algorithm implementation
│   ├── Evaluator.hpp        # Evaluator class declaration
│   ├── Evaluator.cpp        # Postfix evaluation (scalar & matrix)
│   ├── PrettyPrint.h        # Pretty-printer class declaration
│   ├── PrettyPrint.cpp      # Vertically-centered matrix/equation rendering
│   └── IO.h                 # Cross-platform terminal input utilities
├── tests/
│   ├── test_main.cpp        # GoogleTest main entry point
│   ├── test_token.cpp       # Token type system unit tests
│   ├── test_lexer.cpp       # Lexer unit tests
│   ├── test_parser.cpp      # Parser (Shunting-Yard) unit tests
│   ├── test_evaluator.cpp   # Evaluator unit tests
│   ├── test_integration.cpp # End-to-end integration tests
│   └── test_matrix.cpp      # Matrix class unit tests
└── .gitignore               # Git ignore rules
```

---

## 🧪 How It Works — Step by Step

### Scalar Example

Let's trace `(2 + 3) * 4` through the engine:

**1. Lexing** — `Lexer::Tokenize("(2 + 3) * 4")`

```
Tokens: [ LeftParen, 2, Add, 3, RightParen, Mul, 4 ]
```

**2. Parsing** — `Parser::ToPostfix(tokens)` _(Shunting-Yard)_

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

### Matrix Example

Tracing `[1 2; 3 4] * [5; 6]`:

**1. Lexing** — the lexer recognizes `[...]` as a matrix literal:

```
Tokens: [ Matrix(2×2), Mul, Matrix(2×1) ]
         [[1,2],[3,4]]       [[5],[6]]
```

**2. Parsing** — matrices pass through as operands:

```
Postfix: [ Matrix(2×2), Matrix(2×1), * ]
```

**3. Evaluation** — matrix multiplication is dispatched via `std::visit`:

```
Stack trace:
  push [[1,2],[3,4]]  → [M1]
  push [[5],[6]]      → [M1, M2]
  apply *             → [M3]     (M1 × M2)

Result: [ 17 ]
        [ 39 ]
```

### Linear System Solve Example

Tracing `[2 1; 5 3] \ [8; 21]` (solve Ax = b):

**1. Lexing** — the lexer recognizes matrix literals and `\` as InvMul:

```
Tokens: [ Matrix(2×2), InvMul, Matrix(2×1) ]
         [[2,1],[5,3]]          [[8],[21]]
```

**2. Parsing** — matrices pass through as operands:

```
Postfix: [ Matrix(2×2), Matrix(2×1), InvMul ]
```

**3. Evaluation** — `InvMul` calls `Matrix::Solve(A, b)` using LU decomposition:

```
Stack trace:
  push [[2,1],[5,3]]  → [A]
  push [[8],[21]]      → [A, b]
  apply \              → [x]     (Solve Ax = b via LU)

Result: [ 3 ]
        [ 2 ]
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

| Decision                                               | Rationale                                                                                                              |
| ------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
| **Bitmask enum for `TokenType`**                       | Enables fast category checks via bitwise AND (e.g., `type & MathFunctions`)                                            |
| **`std::variant<double, Matrix<double>>` for `Value`** | Unified type-safe value representation — the engine seamlessly handles scalars and matrices through the same pipeline  |
| **`overloaded` visitor pattern**                       | Clean dispatch of mixed-type binary operations (matrix×scalar, scalar+matrix, etc.) without manual type-checking       |
| **`\` backslash (InvMul) operator**                    | MATLAB-inspired `A \ b` syntax for solving linear systems; dispatches to `Matrix::Solve` via LU decomposition          |
| **Header-only `Matrix<T>` template**                   | Generic matrix class supporting any numeric type; all operator overloads defined inline for zero-overhead abstractions |
| **Bracket `[...]` matrix literal syntax**              | MATLAB-inspired inline notation (`[1 2; 3 4]`), parsed directly by the Lexer into `Matrix<double>` tokens              |
| **Shunting-Yard algorithm**                            | Industry-standard O(n) algorithm for infix-to-postfix conversion                                                       |
| **Degrees for trigonometry**                           | More intuitive for end-users (internal conversion: `value × π / 180`)                                                  |
| **Static `Tokenize` / `ToPostfix` / `Evaluate`**       | Functional-style API — no need to manage object lifecycle                                                              |
| **Context-based unary detection**                      | Parser detects unary `-`/`+` when no operand precedes them, avoiding the need for a separate pre-processing pass       |
| **`PrettyPrint` with vertical centering**              | Multi-line matrix output is vertically aligned with surrounding operators for readable equation display                |
| **GoogleTest via FetchContent**                        | Zero-config testing — CMake downloads GoogleTest automatically, no manual setup required                               |

---

## 🛣️ Roadmap

- [ ] Global exception handler for malformed expressions
- [x] Support for negative numbers and unary minus
- [x] Unary plus support
- [x] Modulus operator (`mod`)
- [x] Factorial operator (`!`)
- [x] Percent operator (`%`)
- [x] Pi constant (`pi`) and Euler's number (`e`)
- [x] Unit and integration test suite (GoogleTest)
- [x] Additional functions (`log`, `ln`, `logbase`, `abs`)
- [x] Matrix support — inline literals, arithmetic, transpose, identity, power
- [x] Pretty-printed output with vertically-centered matrix rendering
- [x] Matrix determinant and inverse (Gauss-Jordan elimination)
- [x] Linear system solver (`A \ b`) via LU decomposition with partial pivoting
- [ ] Variable support (`x = 5; 2*x + 3`)
- [ ] Interactive REPL mode

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
