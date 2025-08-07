# Lox C++ Tree-Walker Interpreter

⚠️ **Note: This implementation is under development, doesn't use extensive testing and may be buggy.**

A C++ implementation of the tree-walking interpreter from [*Crafting Interpreters*](https://craftinginterpreters.com/), covering the core Lox language features through basic OOP (no inheritance or `this` keyword).

## Features

- Variables, functions, and closures
- Control flow (`if`, `while`, `for`)
- Classes with methods and fields
- Dynamic typing and lexical scoping
- Runtime error handling

## Building

```bash
cd build
cmake ..
make
```

## Usage

```bash
build/CppLox              # Interactive REPL
build/CppLox script.lox   # Execute file
```

## Example

```javascript
fun fibonacci(n) {
  if (n <= 1) return n;
  return fibonacci(n - 2) + fibonacci(n - 1);
}

print fibonacci(5);
```

## About

Educational project implementing the jlox interpreter from Part II of *Crafting Interpreters* in C++. Uses recursive descent parsing and tree-walking evaluation.

*Based on Robert Nystrom's [Crafting Interpreters](https://craftinginterpreters.com/)*
