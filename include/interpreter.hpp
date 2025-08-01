#pragma once

#include "expr.hpp"
#include "token.hpp"

struct Interpreter {
  LiteralObject operator()(Unary unary) const;

  LiteralObject operator()(Binary binary) const;

  LiteralObject operator()(Grouping grouping) const;

  LiteralObject operator()(Literal literal) const;

  void interpret(Expr expression);
};
