#pragma once

#include "expr.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include <string>
#include <unordered_map>

enum FunctionType { NONE, FUNCTION };

class Resolver {
private:
  Interpreter &interpreter;
  std::vector<std::unordered_map<std::string, bool>> scopes{};
  FunctionType currentFunction = NONE;

public:
  static size_t counter;

  Resolver(Interpreter &interpreter);

  void operator()(Block &block);

  void operator()(Class &klass);

  void operator()(Var &stmt);

  void operator()(Expression &stmt);

  void operator()(If &stmt);

  void operator()(Print &stmt);

  void operator()(Return &stmt);

  void operator()(While &stmt);

  void operator()(Func &stmt);

  void operator()(Variable &expr);

  void operator()(Assign &expr);

  void operator()(Binary &expr);

  void operator()(Call &expr);

  void operator()(Grouping &expr);

  void operator()(Literal &expr);

  void operator()(Logical &expr);

  void operator()(Unary &expr);

  void operator()(Get &expr);

  void operator()(Set &expr);

  void declare(Token name);

  void define(Token name);

  void resolve(std::vector<std::shared_ptr<Stmt>> statements);

  void resolve(std::shared_ptr<Stmt> statement);

  void resolve(std::shared_ptr<Expr> expr);

  void resolveLocal(std::shared_ptr<Expr> expr, Token name, size_t id);

  void resolveFunction(Func &stmt, FunctionType type);

  void beginScope();

  void endScope();
};
