#pragma once

#include "token.hpp"
#include <memory>
#include <variant>

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

using Expr = std::variant<Binary, Grouping, Literal, Unary, std::monostate>;

struct Binary {
  std::shared_ptr<Expr> left;
  Token op;
  std::shared_ptr<Expr> right;

  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}
};

struct Grouping {
  std::shared_ptr<Expr> expression;

  Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}
};

struct Literal {
  LiteralObject value;

  Literal(LiteralObject value) : value(value) {}
};

struct Unary {
  Token op;
  std::shared_ptr<Expr> right;

  Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}
};
