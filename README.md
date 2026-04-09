<div align="center">

```
 ██████╗██████╗ ██████╗ ██╗      ██████╗ ██╗  ██╗
██╔════╝██╔══██╗██╔══██╗██║     ██╔═══██╗╚██╗██╔╝
██║     ██████╔╝██████╔╝██║     ██║   ██║ ╚███╔╝ 
██║     ██╔═══╝ ██╔═══╝ ██║     ██║   ██║ ██╔██╗ 
╚██████╗██║     ██║     ███████╗╚██████╔╝██╔╝ ██╗
 ╚═════╝╚═╝     ╚═╝     ╚══════╝ ╚═════╝ ╚═╝  ╚═╝
```

### A complete tree-walking interpreter for the **Lox** language, hand-crafted in modern C++.
### *Porting Robert Nystrom's* [Crafting Interpreters](https://craftinginterpreters.com/) *jlox from Java to idiomatic C++17/20.*

<br/>

![C++](https://img.shields.io/badge/C%2B%2B-17%2F20-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)
![Book](https://img.shields.io/badge/Based%20On-Crafting%20Interpreters-orange?style=for-the-badge)

</div>

---

## 📖 What is this?

**cpplox** is a full implementation of the **Lox** scripting language — a dynamically-typed, garbage-collected language designed by Robert Nystrom in his book [*Crafting Interpreters*](https://craftinginterpreters.com/). The original book implements the interpreter ("jlox") in Java. This project ports the entire pipeline to **modern C++**, replacing Java idioms with idiomatic C++17/20 equivalents: `std::variant` over inheritance hierarchies, `std::optional` over null sentinels, RAII over manual memory management, and the Visitor pattern implemented cleanly via `std::visit`.

This is not a copy-paste port. Every design decision was made consciously — choosing the right C++ feature for each job and writing code that would make a C++ engineer nod, not wince.

---

## 🛠️ Language Features Supported

Lox is a surprisingly complete little language. cpplox supports all of it:

| Feature | Status |
|---|---|
| Arithmetic & comparison operators | ✅ |
| String concatenation | ✅ |
| Boolean logic (`and`, `or`, `not`) | ✅ |
| Variables (`var`) | ✅ |
| Control flow (`if` / `else`, `while`, `for`) | ✅ |
| Functions & closures | ✅ |
| Classes, methods & `this` | ✅ |
| Inheritance (`super`) | ✅ |
| Lexical scoping & variable resolution | ✅ |
| Runtime error reporting | ✅ |
| REPL (interactive mode) | ✅ |
| Script file execution | ✅ |

---

## 🏗️ Architecture

The interpreter is a classic **pipeline architecture**, where source text is progressively transformed into a result:

```
Source Text
    │
    ▼
┌─────────┐
│ Scanner │  Tokenises raw source into a flat list of Tokens.
│ (Lexer) │  Handles keywords, literals, identifiers, and lexical errors.
└────┬────┘
     │  Vec<Token>
     ▼
┌────────┐
│ Parser │  Recursive-descent parser. Consumes tokens and builds an
│        │  Abstract Syntax Tree (AST) from Expr and Stmt nodes.
└────┬───┘
     │  AST (Expr / Stmt tree)
     ▼
┌──────────┐
│ Resolver │  A semantic analysis pass that walks the AST before execution.
│          │  Resolves variable bindings, catches use-before-define,
│          │  and checks for invalid `return` / `this` usage.
└────┬─────┘
     │  Annotated AST + resolution map
     ▼
┌─────────────┐
│ Interpreter │  Tree-walking evaluator. Visits each AST node, evaluates
│             │  expressions, executes statements, and manages the runtime
│             │  environment (scope chain). Throws typed C++ exceptions
│             │  for runtime errors and control flow (return values).
└─────────────┘
     │
     ▼
  Output / Side Effects
```

### Key Design Patterns

- **Visitor Pattern** — The AST node hierarchy uses `std::variant<...>` for expression and statement types. Evaluation and resolution are implemented as `std::visit` calls with overloaded lambdas or callable structs — no raw virtual dispatch.
- **RAII Environments** — Scopes are managed via a linked chain of `Environment` objects wrapped in `std::shared_ptr`, ensuring clean lifetime semantics for closures that outlive their defining scope.
- **`std::optional` for nullable values** — Replaces Java's `null` sentinel returns with explicit optionality in the C++ type system.
- **Typed exceptions for control flow** — `return` statements are implemented as C++ exceptions carrying the return value, cleanly unwinding the call stack without needing explicit return-flag threading.

---

## 📁 Project Structure

```
cpplox/
├── CMakeLists.txt          # Coordinates AST metaprogramming and main build
├── include/                # Public headers defining the interfaces
│   ├── AstPrinter.h        # AST stringification using std::visit and fold expressions
│   ├── Environment.h       # Lexical scope state
│   ├── Files.h             # File I/O utilities
│   ├── Interpreter.h       # Tree-walking evaluator
│   ├── Lox.h               # Main driver class
│   ├── LoxCallable.h       # Interface for functions and classes
│   ├── LoxFunction.h       # Runtime representation of Lox functions
│   ├── Parser.h            # Tokens -> AST
│   ├── Resolver.h          # Semantic analysis pass
│   ├── ReturnException.h   # Exception for control flow
│   ├── RuntimeError.h      # Exception types
│   ├── Scanner.h           # Source text -> Token stream
│   ├── Token.h             # Token definitions and types
│   └── TokenType.h         # Enum for token types
├── src/                    # Implementation files
│   ├── Environment.cpp
│   ├── Files.cpp
│   ├── Interpreter.cpp
│   ├── Lox.cpp             # Top-level driver logic
│   ├── LoxFunction.cpp
│   ├── main.cpp            # Entry point, REPL, and file runner
│   ├── Parser.cpp
│   ├── Resolver.cpp
│   ├── Scanner.cpp
│   └── Token.cpp
└── tools/
    └── GenerateAst.cpp     # C++ metaprogramming tool that writes the AST headers
```

---

## 🚀 Getting Started

### Prerequisites

- A C++17-compatible compiler: **GCC 10+**, **Clang 10+**, or **MSVC 2019+**
- **CMake 3.20+**
- **Git**

### Build Instructions

```bash
# 1. Clone the repository
git clone https://github.com/FaiqAbdullah2B/cpplox.git
cd cpplox

# 2. Create a build directory
mkdir build && cd build

# 3. Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. Build
cmake --build . --config Release

```

### Running cpplox

**Interactive REPL** — type Lox expressions and see results immediately:
```bash
./cpplox
```
```
cpplox> print "Hello, World!";
Hello, World!
cpplox> var x = 10 + 5 * 2;
cpplox> print x;
20
cpplox>
```

**Execute a Lox script:**
```bash
./cpplox path/to/script.lox
```

---

## 🧪 Sample Lox Programs

**Closures:**
```lox
fun makeCounter() {
  var count = 0;
  fun increment() {
    count = count + 1;
    print count;
  }
  return increment;
}

var counter = makeCounter();
counter(); // 1
counter(); // 2
counter(); // 3
```

**Classes & Inheritance:**
```lox
class Animal {
  init(name) {
    this.name = name;
  }
  speak() {
    print this.name + " makes a sound.";
  }
}

class Dog < Animal {
  speak() {
    print this.name + " barks.";
  }
}

var d = Dog("Rex");
d.speak(); // Rex barks.
```

**Fibonacci:**
```lox
fun fib(n) {
  if (n < 2) return n;
  return fib(n - 1) + fib(n - 2);
}

print fib(20); // 6765
```

---

## 🧠 What I Learned Building This

This project was a deep dive into how programming languages actually work under the hood. Key takeaways:

### Compiler/Interpreter Theory
- How a **lexer** tokenises source text using finite-state principles
- How a **recursive-descent parser** implements a context-free grammar directly in code — one function per grammar rule
- The difference between **syntax errors** (caught at parse time) and **semantic errors** (caught at resolution/runtime)
- How **lexical scoping** is implemented: the resolver walks the AST and pre-computes variable lookup depths, which the interpreter then uses for O(1) environment access
- How **closures** work at the implementation level: a function object captures a reference to its defining `Environment`, keeping it alive even after the enclosing scope returns

### Modern C++ Engineering
- Using `std::variant` and `std::visit` as a type-safe alternative to virtual dispatch for sum types
- Writing overload sets with `std::visit` using the overloaded-lambda pattern
- Managing object lifetimes with `std::shared_ptr` and `std::weak_ptr` for the scope chain
- Using `std::optional` to express nullable results in a type-safe way
- Exception-based control flow as a clean mechanism for `return` propagation
- Structuring a non-trivial C++ project with CMake and clean header/source separation

### Software Engineering
- How to design a clean pipeline where each stage has a single responsibility
- How one can use **Meta-programming** to write code that writes code.
- Error recovery: continuing to parse after a syntax error to surface multiple errors in one pass
- The value of a **semantic analysis pass** (Resolver) separate from execution — it catches a whole class of errors before any code runs

---

## 📚 Reference

- 📗 **Book**: [Crafting Interpreters by Robert Nystrom](https://craftinginterpreters.com/) — freely available online, one of the best programming books ever written.
- 🔤 **Language Spec**: [The Lox Language](https://craftinginterpreters.com/the-lox-language.html)

---

## 👤 Author

**Faiq Abdullah**
- GitHub: [@FaiqAbdullah2B](https://github.com/FaiqAbdullah2B)

---

<div align="center">

*Built from scratch. No shortcuts. Just a scanner, a parser, a resolver, and sheer stubbornness.*

</div>
