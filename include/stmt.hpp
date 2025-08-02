#pragma once

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <variant>
#include <vector>

struct Block;
struct Expression;
struct Print;
struct Var;

using Stmt = std::variant<Block, Expression, Print, Var>;

struct Block {
  std::vector<std::shared_ptr<Stmt>> statements;

  Block(std::vector<std::shared_ptr<Stmt>> statements)
      : statements(statements) {}
};

struct Expression {
  std::shared_ptr<Expr> expr;

  Expression(std::shared_ptr<Expr> expr) : expr(expr) {}
};

struct Print {
  std::shared_ptr<Expr> expr;

  Print(std::shared_ptr<Expr> expr) : expr(expr) {}
};

struct Var {
  Token name;
  std::shared_ptr<Expr> initializer;

  Var(Token name, std::shared_ptr<Expr> initializer)
      : name(name), initializer(initializer) {}
};
