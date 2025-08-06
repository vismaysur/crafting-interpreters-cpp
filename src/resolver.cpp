#include "resolver.hpp"
#include "error_reporter.hpp"
#include <cwchar>
#include <iostream>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

extern ErrorReporter errorReporter;

size_t Resolver::counter = 0;

Resolver::Resolver(Interpreter &interpreter) : interpreter(interpreter) {}

void Resolver::operator()(Block &block) {
  beginScope();
  resolve(block.statements);
  endScope();
}

void Resolver::operator()(Var &stmt) {
  declare(stmt.name);
  if (stmt.initializer != nullptr) {
    resolve(stmt.initializer);
  }
  define(stmt.name);
}

void Resolver::operator()(Func &stmt) {
  declare(stmt.name);
  define(stmt.name);

  resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::operator()(Expression &stmt) { resolve(stmt.expr); }

void Resolver::operator()(If &stmt) {
  resolve(stmt.condition);
  resolve(stmt.thenBranch);
  if (stmt.elseBranch != nullptr)
    resolve(stmt.elseBranch);
}

void Resolver::operator()(Print &stmt) { resolve(stmt.expr); }

void Resolver::operator()(Return &stmt) {
  if (currentFunction == NONE) {
    errorReporter.error(stmt.keyword, "Can't return from top-level code.");
  }

  if (stmt.value)
    resolve(stmt.value);
}

void Resolver::operator()(While &stmt) {
  resolve(stmt.condition);
  resolve(stmt.body);
}

void Resolver::operator()(Variable &expr) {
  if (!scopes.empty() && scopes.back().count(expr.name.lexeme) &&
      scopes.back()[expr.name.lexeme] == false) {
    errorReporter.error(expr.name,
                        "Can't read local variable in its own initializer.");
  }

  expr.id = counter++;

  resolveLocal(std::make_shared<Expr>(expr), expr.name, expr.id.value());
}

void Resolver::operator()(Assign &expr) {
  expr.id = counter++;

  resolve(expr.value);
  resolveLocal(std::make_shared<Expr>(expr), expr.name, expr.id.value());
}

void Resolver::operator()(Binary &expr) {
  expr.id = counter++;

  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::operator()(Call &expr) {
  expr.id = counter++;

  resolve(expr.callee);

  for (std::shared_ptr<Expr> arg : expr.args) {
    resolve(arg);
  }
}

void Resolver::operator()(Grouping &expr) {
  expr.id = counter++;

  resolve(expr.expression);
}

void Resolver::operator()(Literal &expr) { expr.id = counter++; }

void Resolver::operator()(Logical &expr) {
  expr.id = counter++;

  resolve(expr.left);
  resolve(expr.right);
}

void Resolver::operator()(Unary &expr) {
  expr.id = counter++;

  resolve(expr.right);
}

void Resolver::declare(Token name) {
  if (scopes.empty())
    return;

  std::unordered_map<std::string, bool> &scope = scopes.back();

  if (scope.count(name.lexeme)) {
    errorReporter.error(name,
                        "Already a variable with this name in this scope.");
  }

  scope[name.lexeme] = false;
}

void Resolver::define(Token name) {
  if (scopes.empty())
    return;

  std::unordered_map<std::string, bool> &scope = scopes.back();

  scope[name.lexeme] = true;
}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>> statements) {
  for (std::shared_ptr<Stmt> stmt : statements) {
    resolve(stmt);
  }
}

void Resolver::resolve(std::shared_ptr<Stmt> statement) {
  std::visit(*this, *statement);
}

void Resolver::resolve(std::shared_ptr<Expr> expr) { std::visit(*this, *expr); }

void Resolver::resolveLocal(std::shared_ptr<Expr> expr, Token name, size_t id) {
  for (int i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].count(name.lexeme)) {
      interpreter.resolve(expr, scopes.size() - 1 - i, id);
      return;
    }
  }
}

void Resolver::resolveFunction(Func &stmt, FunctionType type) {
  FunctionType enclosingFunction = currentFunction;
  currentFunction = type;

  beginScope();

  for (Token param : stmt.params) {
    declare(param);
    define(param);
  }

  resolve(stmt.body);

  endScope();

  currentFunction = enclosingFunction;
}

void Resolver::beginScope() {
  scopes.push_back(std::unordered_map<std::string, bool>{});
}

void Resolver::endScope() { scopes.pop_back(); }
