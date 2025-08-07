#pragma once

#include "environment.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include <unordered_map>
#include <vector>

struct Interpreter {
  Interpreter();

  LiteralObject operator()(Assign &assign);

  LiteralObject operator()(Unary &unary);

  LiteralObject operator()(Binary &binary);

  LiteralObject operator()(Call &call);

  LiteralObject operator()(Grouping &grouping);

  LiteralObject operator()(Get &get);

  LiteralObject operator()(Set &set);

  LiteralObject operator()(Literal &literal);

  LiteralObject operator()(Logical &logical);

  LiteralObject operator()(Variable &variable);

  void operator()(Block &stmt);

  void operator()(Class &stmt);

  void operator()(Print &stmt);

  void operator()(If &stmt);

  void operator()(Expression &stmt);

  void operator()(Func &stmt);

  void operator()(Var &stmt);

  void operator()(Return &stmt);

  void operator()(While &stmt);

  void interpret(std::vector<std::shared_ptr<Stmt>> &stmts);

  LiteralObject evaluate(Expr &expr);

  void executeBlock(std::vector<std::shared_ptr<Stmt>> statements,
                    std::shared_ptr<Environment> environment);

  void resolve(std::shared_ptr<Expr> expr, int hops, size_t id);

  LiteralObject lookUpVariable(Token name, Variable &expr);

  std::shared_ptr<Environment> globals = std::make_shared<Environment>();
  std::shared_ptr<Environment> environment = globals;
  std::unordered_map<size_t, int> locals{};
};
