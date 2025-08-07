#pragma once

#include "token.hpp"
#include <memory>
#include <optional>
#include <variant>
#include <vector>

struct Assign;
struct Logical;
struct Binary;
struct Call;
struct Get;
struct Set;
struct Grouping;
struct Literal;
struct Unary;
struct Variable;

using Expr = std::variant<Assign, Logical, Binary, Call, Get, Set, Grouping,
                          Literal, Unary, Variable>;

struct Assign {
  std::optional<size_t> id;
  Token name;
  std::shared_ptr<Expr> value;

  Assign(Token name, std::shared_ptr<Expr> value) : name(name), value(value) {}
};

struct Logical {
  std::optional<size_t> id;
  std::shared_ptr<Expr> left;
  Token op;
  std::shared_ptr<Expr> right;

  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}
};

struct Binary {
  std::optional<size_t> id;
  std::shared_ptr<Expr> left;
  Token op;
  std::shared_ptr<Expr> right;

  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}
};

struct Call {
  std::optional<size_t> id;
  std::shared_ptr<Expr> callee;
  Token paren;
  std::vector<std::shared_ptr<Expr>> args;

  Call(std::shared_ptr<Expr> callee, Token paren,
       std::vector<std::shared_ptr<Expr>> args)
      : callee(callee), paren(paren), args(args) {}
};

struct Get {
  std::shared_ptr<Expr> object;
  Token name;

  Get(std::shared_ptr<Expr> object, Token name) : object(object), name(name) {}
};

struct Set {
  std::shared_ptr<Expr> object;
  Token name;
  std::shared_ptr<Expr> value;

  Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
      : object(object), name(name), value(value) {}
};

struct Grouping {
  std::optional<size_t> id;
  std::shared_ptr<Expr> expression;

  Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}
};

struct Literal {
  std::optional<size_t> id;
  LiteralObject value;

  Literal(LiteralObject value) : value(value) {}
};

struct Unary {
  std::optional<size_t> id;
  Token op;
  std::shared_ptr<Expr> right;

  Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}
};

struct Variable {
  std::optional<size_t> id;
  Token name;

  Variable(Token name) : name(name) {}
};
