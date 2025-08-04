#pragma once

#include "environment.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include <vector>

struct Interpreter {
  Interpreter();

  LiteralObject operator()(Assign assign) const;

  LiteralObject operator()(Unary unary) const;

  LiteralObject operator()(Binary binary) const;

  LiteralObject operator()(Call call) const;

  LiteralObject operator()(Grouping grouping) const;

  LiteralObject operator()(Literal literal) const;

  LiteralObject operator()(Logical logical) const;

  LiteralObject operator()(Variable variable) const;

  void operator()(Block stmt);

  void operator()(Print stmt) const;

  void operator()(If stmt);

  void operator()(Expression stmt) const;

  void operator()(Func stmt);

  void operator()(Var stmt) const;

  void operator()(Return stmt);

  void operator()(While stmt);

  void interpret(std::vector<std::unique_ptr<Stmt>> &stmts);

  LiteralObject evaluate(Expr expr) const;

  void executeBlock(std::vector<std::shared_ptr<Stmt>> statements,
                    std::shared_ptr<Environment> environment);

  std::shared_ptr<Environment> globals = std::make_shared<Environment>();
  std::shared_ptr<Environment> environment = globals;
};
