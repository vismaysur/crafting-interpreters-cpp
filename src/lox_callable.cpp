#include "lox_callable.hpp"
#include "environment.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include <ctime>
#include <iostream>
#include <memory>

// LoxCallable

// int LoxCallable::arity() { return 0; }
//
// LiteralObject LoxCallable::call(Interpreter interpreter,
//                                 std::vector<LiteralObject> args) {
//   return std::monostate{};
// }
//
// std::string LoxCallable::toString() { return "<undefined callable>"; }

// ClockFunc

int ClockFunc::arity() { return 0; }

LiteralObject ClockFunc::call(Interpreter interpreter,
                              std::vector<LiteralObject> args) {
  return static_cast<double>(std::time(nullptr));
}

std::string ClockFunc::toString() const { return "<native fn>"; }

// LoxFunc

LoxFunc::LoxFunc(std::shared_ptr<Func> funcDeclaration,
                 std::shared_ptr<Environment> closure) {
  this->funcDeclaration = funcDeclaration;
  this->closure = closure;
}

int LoxFunc::arity() { return funcDeclaration->params.size(); }

LiteralObject LoxFunc::call(Interpreter interpreter,
                            std::vector<LiteralObject> args) {
  std::shared_ptr<Environment> env = std::make_shared<Environment>(closure);

  for (int i = 0; i < funcDeclaration->params.size(); i++) {
    env->define(funcDeclaration->params[i].lexeme, args[i]);
  }

  interpreter.executeBlock(funcDeclaration->body, env);

  return std::monostate{};
}

std::string LoxFunc::toString() const {
  return "<fn " + funcDeclaration->name.lexeme + ">";
}
