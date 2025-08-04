#pragma once

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <variant>
#include <vector>

struct Block;
struct Expression;
struct Func;
struct If;
struct Print;
struct Return;
struct Var;
struct While;

using Stmt =
    std::variant<Block, Expression, Func, If, Print, Return, Var, While>;

struct Block {
  std::vector<std::shared_ptr<Stmt>> statements;

  Block(std::vector<std::shared_ptr<Stmt>> statements)
      : statements(statements) {}
};

struct Expression {
  std::shared_ptr<Expr> expr;

  Expression(std::shared_ptr<Expr> expr) : expr(expr) {}
};

struct Func {
  Token name;
  std::vector<Token> params;
  std::vector<std::shared_ptr<Stmt>> body;

  Func(Token name, std::vector<Token> params,
       std::vector<std::shared_ptr<Stmt>> body)
      : name(name), params(params), body(body) {}
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

struct Return {
  Token keyword;
  std::shared_ptr<Expr> value;

  Return(Token keyword, std::shared_ptr<Expr> value)
      : keyword(keyword), value(value) {}
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
