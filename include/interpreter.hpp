#pragma once

#include "environment.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include <vector>

struct Interpreter {
  LiteralObject operator()(Assign assign) const;

  LiteralObject operator()(Unary unary) const;

  LiteralObject operator()(Binary binary) const;

  LiteralObject operator()(Grouping grouping) const;

  LiteralObject operator()(Literal literal) const;

  LiteralObject operator()(Variable variable) const;

  void operator()(Block stmt);

  void operator()(Print stmt) const;

  void operator()(Expression stmt) const;

  void operator()(Var stmt) const;

  void interpret(std::vector<std::unique_ptr<Stmt>> &stmts);

  LiteralObject evaluate(Expr expr) const;

  void executeBlock(std::vector<std::shared_ptr<Stmt>> statements,
                    std::shared_ptr<Environment> environment);

private:
  std::shared_ptr<Environment> environment = std::make_shared<Environment>();
};
