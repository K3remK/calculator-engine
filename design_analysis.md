# Calculator-Engine Design Analysis

## Overall Architecture — ✅ Good Foundation

The **Lexer → Parser → Evaluator** pipeline is a solid, textbook approach. Clean separation of concerns, and each component has a well-defined single responsibility. This is genuinely well-architected for a project at this stage.

---

## 🔴 Active Bugs

### 1. `Matrix::Transpose()` — Wrong Loop Bounds

```cpp
// Current (WRONG):
for (size_t row = 0; row < GetN(); row++) {       // loops over columns
    for (size_t col = 0; col < GetM(); col++) {    // loops over rows
        res[col][row] = m_Data[row][col];           // indices are also swapped
    }
}
```

This accidentally works for square matrices but crashes or produces wrong results for non-square matrices (e.g. 3×2 → 2×3). Fix:

```cpp
for (size_t row = 0; row < GetM(); row++) {
    for (size_t col = 0; col < GetN(); col++) {
        res[col][row] = m_Data[row][col];
    }
}
```

### 2. Free Operators `val - m` and `val / m` — Mathematically Wrong

```cpp
// Line 255: val - m  returns  m - val  (WRONG: subtraction is NOT commutative)
Matrix<T> operator-(A val, const Matrix<T>& m) { return m - val; }

// Line 260: val / m  returns  m / val  (WRONG: division is NOT commutative)
Matrix<T> operator/(A val, const Matrix<T>& m) { return m / val; }
```

`5 - [[1,2],[3,4]]` should give `[[4,3],[2,1]]`, but currently gives `[[-4,-3],[-2,-1]]`.

### 3. [maxVariadic](file:///home/kerem/Desktop/dev/calculator-engine/src/Evaluator.cpp#81-102) Uses `DBL_MIN` — Wrong Constant

```cpp
auto currentMax = DBL_MIN;  // This is the smallest POSITIVE normalized double (~2.2e-308)
```

This means [max(-1, -2, -3)](file:///home/kerem/Desktop/dev/calculator-engine/src/Evaluator.cpp#81-102) would return `DBL_MIN` ≈ `2.2e-308` instead of `-1`. Use `std::numeric_limits<double>::lowest()` or `-DBL_MAX`.

### 4. [Util.hpp](file:///home/kerem/Desktop/dev/calculator-engine/src/Util.hpp) — Entirely Broken (Unused)

```cpp
// get_number: index++ as side effect inside condition → skips characters
while (index < str.length() && (std::isdigit(str[index]) || str[index++] == '.'))
//                                                           ^^^^^^^ BUG

// get_string: returns reference to a local variable → undefined behavior
static const std::string& get_string(...) {
    auto word = "";  // const char*, not even std::string
    ...
    return word;     // dangling reference
}
```

Both appear unused (Lexer has its own implementation). Recommend deleting this file entirely.

### 5. [main.cpp](file:///home/kerem/Desktop/dev/calculator-engine/src/main.cpp) — `run` Starts as `false`

```cpp
bool run = false;  // Line 105 — the interactive REPL loop on line 108 never executes
```

---

## 🟡 Design Concerns

### 1. `mutable` Fields on Token — Smell

```cpp
struct Token {
    mutable TokenType type;   // modified by Parser to convert Sub→UnaryMinus
    mutable Value data;
    mutable std::size_t argc;
```

`mutable` is supposed to mean "logically const but physically changeable" (like a cache). Here it's used because the Parser mutates tokens it receives as `const&`. This breaks const-correctness. The real fix: the Parser should produce *new* tokens instead of mutating input tokens.

### 2. `static` Globals in Headers — ODR Risk

Both `operatorMap` (Token.hpp) and `typeMapper` (Lexer.hpp) are `static` in headers. Every translation unit gets its own copy → wasted memory and potential inconsistency. Move them to [.cpp](file:///home/kerem/Desktop/dev/calculator-engine/src/main.cpp) files or use `inline`.

### 3. TokenType Bitmask — 32-Bit Limit

```cpp
enum TokenType : uint32_t {
    MatrixT = 1 << 27,  // Already at bit 27 — only 4 bits left
};
```

You're running out of bits. Adding 5 more tokens will overflow. Consider switching to `uint64_t`, or rethink whether every type needs to be a bitmask (only the ones used in bitmask checks like `Numbers | PostfixOperators` need it).

### 4. `Parser::ToPostfix` — Too Large (~125 lines)

This single function handles number validation, unary detection, function argc tracking, operator precedence, and parenthesis matching. Extracting helpers would make it more readable:

```
parseNumber(), parseFunction(), parseOperator(), parseParenthesis()
```

### 5. `assert()` in Parser — Stripped in Release

```cpp
assert(!stack.empty());       // Line 66 — silently skipped in release
assert(stack.top().type == LeftParen);  // Line 77
```

These should be `if (...) throw` for production safety.

### 6. `is_same_v<T, int>` Checks Are Dead Code

In [evalUnary](file:///home/kerem/Desktop/dev/calculator-engine/src/Evaluator.cpp#142-191), several branches check `std::is_same_v<T, int>`, but `Value = std::variant<double, Matrix<double>>` — there is no [int](file:///home/kerem/Desktop/dev/calculator-engine/src/IO.h#24-27) alternative. These checks never match.

### 7. Power of Matrices — Noted as Wrong

```cpp
// Evaluator.cpp line 119 — comment says "WRONG CALCULATION"
for (int i = 0; i < a; i++)
    result = result * m;
```

This computes `Identity × m^a` using repeated multiplication starting from all-ones matrix (not identity). The initial value [Matrix(m, n, 1)](file:///home/kerem/Desktop/dev/calculator-engine/src/Matrix.h#14-243) should be `m.Identity()`.

---

## 🟢 Suggestions for Improvement

| Priority | Suggestion |
|----------|-----------|
| **High** | Delete [Util.hpp](file:///home/kerem/Desktop/dev/calculator-engine/src/Util.hpp) — it's unused and broken |
| **High** | Fix the `DBL_MIN` bug in [maxVariadic](file:///home/kerem/Desktop/dev/calculator-engine/src/Evaluator.cpp#81-102) |
| **High** | Fix [Matrix](file:///home/kerem/Desktop/dev/calculator-engine/src/Matrix.h#14-243) operator `val - m` and `val / m` |
| **Medium** | Remove `mutable` from Token — have Parser create new tokens |
| **Medium** | Move `operatorMap`/`typeMapper` to [.cpp](file:///home/kerem/Desktop/dev/calculator-engine/src/main.cpp) files or use `inline` |
| **Medium** | Replace `assert()` with exceptions in Parser |
| **Medium** | Switch `TokenType` to `uint64_t` to avoid running out of bits |
| **Low** | Break up `Parser::ToPostfix` into smaller functions |
| **Low** | Remove dead [int](file:///home/kerem/Desktop/dev/calculator-engine/src/IO.h#24-27) type checks in evaluator |
| **Low** | Add input validation for trig edge cases (e.g., `tan(90°)`) |

---

## Summary

The core architecture is **solid and well-thought-out**. The Lexer→Parser→Evaluator pipeline, the shunting-yard algorithm, and the variant-based `Value` type are all good design choices. The Matrix support is a nice extension.

The main issues are **implementation bugs** (matrix ops, `DBL_MIN`, [Util.hpp](file:///home/kerem/Desktop/dev/calculator-engine/src/Util.hpp)) and **code hygiene** (`mutable`, `static` in headers, `assert`). None of these are fundamental design flaws — they're all fixable without restructuring.
