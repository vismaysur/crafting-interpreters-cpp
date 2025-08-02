#pragma once

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include <vector>

struct ExpressionInterpreter {
  LiteralObject operator()(Unary unary) const;

  LiteralObject operator()(Binary binary) const;

  LiteralObject operator()(Grouping grouping) const;

  LiteralObject operator()(Literal literal) const;
};

struct StatementInterpreter {
  void operator()(Print stmt) const;

  void operator()(Expression stmt) const;

  void interpret(std::vector<std::unique_ptr<Stmt>> &stmts);
};
