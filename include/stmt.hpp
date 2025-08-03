#pragma once

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <variant>
#include <vector>

struct Block;
struct Expression;
struct If;
struct Print;
struct Var;
struct While;

using Stmt = std::variant<Block, Expression, If, Print, Var, While>;

struct Block {
  std::vector<std::shared_ptr<Stmt>> statements;

  Block(std::vector<std::shared_ptr<Stmt>> statements)
      : statements(statements) {}
};

struct Expression {
  std::shared_ptr<Expr> expr;

  Expression(std::shared_ptr<Expr> expr) : expr(expr) {}
};

struct If {
  std::shared_ptr<Expr> condition;
  std::shared_ptr<Stmt> thenBranch;
  std::shared_ptr<Stmt> elseBranch;

  If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch,
     std::shared_ptr<Stmt> elseBranch)
      : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
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

struct While {
  std::shared_ptr<Expr> condition;
  std::shared_ptr<Stmt> body;

  While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
      : condition(condition), body(body) {}
};
