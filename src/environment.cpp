#include "environment.hpp"
#include "lox_callable.hpp"
#include "runtime_error.hpp"
#include "token.hpp"
#include <iostream>
#include <unordered_map>

Environment::Environment() { enclosing = nullptr; }

Environment::Environment(std::shared_ptr<Environment> enclosing) {
  this->enclosing = enclosing;
}

void Environment::define(std::string name, LiteralObject value) {
  values.insert({name, value});
}

void Environment::assign(Token name, LiteralObject value) {
  if (values.count(name.lexeme)) {
    values[name.lexeme] = value;
    return;
  }

  if (enclosing != nullptr)
    return enclosing->assign(name, value);

  throw new RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assignAt(int distance, Token name, LiteralObject value) {
  ancestor(distance)->values[name.lexeme] = value;
}

LiteralObject Environment::get(Token name) {
  if (values.count(name.lexeme)) {
    return values[name.lexeme];
  }

  if (enclosing != nullptr) {
    return enclosing->get(name);
  }

  throw new RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

LiteralObject Environment::getAt(int distance, Token name) {
  return ancestor(distance)->values[name.lexeme];
}

Environment *Environment::ancestor(int distance) {
  Environment *env = this;

  for (int i = 0; i < distance; i++) {
    env = env->enclosing.get();
  }

  return env;
}
