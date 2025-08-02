#pragma once

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <variant>

struct Expression;
struct Print;

using Stmt = std::variant<Expression, Print>;

struct Expression {
  std::shared_ptr<Expr> expr;

  Expression(std::shared_ptr<Expr> expr) : expr(expr) {}
};

struct Print {
  std::shared_ptr<Expr> expr;

  Print(std::shared_ptr<Expr> expr) : expr(expr) {}
};
