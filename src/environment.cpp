#include "environment.hpp"
#include "runtime_error.hpp"
#include "token.hpp"
#include <unordered_map>

void Environment::define(std::string name, LiteralObject value) {
  values.insert({name, value});
}

void Environment::assign(Token name, LiteralObject value) {
  if (values.count(name.lexeme)) {
    values[name.lexeme] = value;
    return;
  }

  throw new RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

LiteralObject Environment::get(Token name) {
  if (values.count(name.lexeme)) {
    return values[name.lexeme];
  }

  throw new RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
