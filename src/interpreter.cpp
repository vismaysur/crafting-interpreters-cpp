#include "interpreter.hpp"
#include "error_reporter.hpp"
#include "expr.hpp"
#include "func_return.hpp"
#include "lox_callable.hpp"
#include "runtime_error.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <variant>

extern ErrorReporter errorReporter;

void checkNumberOperand(Token op, LiteralObject obj) {
  if (std::holds_alternative<double>(obj))
    return;
  throw new RuntimeError(op, "Operand must be a number.");
}

void checkNumberOperands(Token op, LiteralObject obj1, LiteralObject obj2) {
  if (std::holds_alternative<double>(obj1) &&
      std::holds_alternative<double>(obj2))
    return;
  throw new RuntimeError(op, "Operands must be numbers.");
}

Interpreter::Interpreter() {
  std::shared_ptr<ClockFunc> clockFunc = std::make_shared<ClockFunc>();

  globals->define("clock", clockFunc);
}

LiteralObject Interpreter::operator()(Assign &assign) {
  LiteralObject value = evaluate(*assign.value);

  if (assign.id.has_value() && locals.count(assign.id.value())) {
    int distance = locals[assign.id.value()];
    environment->assignAt(distance, assign.name, value);
  } else {
    globals->assign(assign.name, value);
  }

  return value;
}

LiteralObject Interpreter::operator()(Literal &literal) {
  return literal.value;
}

LiteralObject Interpreter::operator()(Grouping &grouping) {
  return evaluate(*grouping.expression);
}

LiteralObject Interpreter::operator()(Unary &unary) {
  LiteralObject right = evaluate(*unary.right);

  switch (unary.op.type) {
  case TokenType::MINUS:
    checkNumberOperand(unary.op, right);
    return -std::get<double>(right);
  case TokenType::BANG:
    return !std::visit(TruthyLiteralVisitor{}, right);
  default:
    break;
  }

  return std::monostate{};
}

LiteralObject Interpreter::operator()(Binary &binary) {
  LiteralObject left = evaluate(*binary.left);
  LiteralObject right = evaluate(*binary.right);

  switch (binary.op.type) {
  case TokenType::EQUAL_EQUAL:
    return left == right;

  case TokenType::BANG_EQUAL:
    return left != right;

  case TokenType::GREATER:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) > std::get<double>(right);

  case TokenType::LESS:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) < std::get<double>(right);

  case TokenType::GREATER_EQUAL:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) >= std::get<double>(right);

  case TokenType::LESS_EQUAL:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) <= std::get<double>(right);

  case TokenType::MINUS:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) - std::get<double>(right);

  case TokenType::PLUS:
    if (std::holds_alternative<double>(left) &&
        std::holds_alternative<double>(right))
      return std::get<double>(left) + std::get<double>(right);

    if (std::holds_alternative<std::string>(left) &&
        std::holds_alternative<std::string>(right))
      return std::get<std::string>(left) + std::get<std::string>(right);

    throw new RuntimeError(binary.op,
                           "Operands must be two numbers or two strings.");

  case TokenType::STAR:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) * std::get<double>(right);

  case TokenType::SLASH:
    checkNumberOperands(binary.op, left, right);
    return std::get<double>(left) / std::get<double>(right);

  default:
    break;
  }

  return std::monostate{};
}

LiteralObject Interpreter::operator()(Call &expr) {
  LiteralObject callee = evaluate(*expr.callee);

  std::vector<LiteralObject> args{};

  for (std::shared_ptr<Expr> arg : expr.args) {
    args.push_back(evaluate(*arg));
  }

  if (!std::holds_alternative<std::shared_ptr<LoxCallable>>(callee)) {
    throw new RuntimeError(expr.paren, "Can only call functions and classes.");
  }

  std::shared_ptr<LoxCallable> function =
      std::get<std::shared_ptr<LoxCallable>>(callee);

  if (args.size() != function->arity()) {
    throw new RuntimeError(
        expr.paren, "Expected " + std::to_string(function->arity()) +
                        " args, got " + std::to_string(args.size()) + ".");
  }

  LiteralObject retObj = std::monostate{};

  try {
    retObj = function->call(*this, args);
  } catch (FuncReturn *retVal) {
    return retVal->value;
  }

  return retObj;
}

LiteralObject Interpreter::operator()(Logical &expr) {
  LiteralObject left = evaluate(*expr.left);

  if (expr.op.type == TokenType::OR) {
    if (std::visit(TruthyLiteralVisitor{}, left))
      return left;
  } else {
    if (!std::visit(TruthyLiteralVisitor{}, left))
      return left;
  }

  return evaluate(*expr.right);
}

LiteralObject Interpreter::operator()(Get &expr) {
  LiteralObject obj = evaluate(*expr.object);

  if (std::holds_alternative<std::shared_ptr<LoxInstance>>(obj)) {
    return std::get<std::shared_ptr<LoxInstance>>(obj)->get(expr.name);
  }

  throw new RuntimeError(expr.name, "Only instances have properties.");
}

LiteralObject Interpreter::operator()(Set &expr) {
  LiteralObject obj = evaluate(*expr.object);

  if (!std::holds_alternative<std::shared_ptr<LoxInstance>>(obj)) {
    throw new RuntimeError(expr.name, "Only instances have fields.");
  }

  LiteralObject value = evaluate(*expr.value);
  std::get<std::shared_ptr<LoxInstance>>(value)->set(expr.name, value);

  return value;
}

LiteralObject Interpreter::operator()(Variable &expr) {
  return lookUpVariable(expr.name, expr);
}

LiteralObject Interpreter::lookUpVariable(Token name, Variable &expr) {
  if (expr.id.has_value() && locals.count(expr.id.value())) {
    int distance = locals[expr.id.value()];
    LiteralObject res = environment->getAt(distance, name);
    return res;
  } else {
    return globals->get(name);
  }
}

void Interpreter::operator()(Block &stmt) {
  executeBlock(stmt.statements,
               std::make_shared<Environment>(this->environment));
}

void Interpreter::operator()(Class &stmt) {
  environment->define(stmt.name.lexeme, std::monostate{});
  std::shared_ptr<LoxCallable> klass =
      std::make_shared<LoxClass>(stmt.name.lexeme);
  environment->assign(stmt.name, klass);
}

void Interpreter::operator()(Print &stmt) {
  LiteralObject value = evaluate(*stmt.expr);
  std::cout << std::visit(StringifyLiteralVisitor{}, value) << std::endl;
}

void Interpreter::operator()(Func &func) {
  std::shared_ptr<Func> funcPtr =
      std::make_shared<Func>(func.name, func.params, func.body);

  std::shared_ptr<LoxCallable> loxFunc =
      std::make_shared<LoxFunc>(funcPtr, environment);

  environment->define(func.name.lexeme, loxFunc);

  // MEM LEAK :(
}

void Interpreter::operator()(If &stmt) {
  bool conditionTrue =
      std::visit(TruthyLiteralVisitor{}, evaluate(*stmt.condition));

  if (conditionTrue) {
    std::visit(*this, *(stmt.thenBranch));
  } else {
    if (stmt.elseBranch)
      std::visit(*this, *(stmt.elseBranch));
  }
}

void Interpreter::operator()(Expression &stmt) { evaluate(*stmt.expr); }

void Interpreter::operator()(Var &stmt) {
  LiteralObject value = std::monostate{};
  if (stmt.initializer != nullptr) {
    value = evaluate(*stmt.initializer);
  }

  environment->define(stmt.name.lexeme, value);
}

void Interpreter::operator()(While &stmt) {
  while (std::visit(TruthyLiteralVisitor{}, evaluate(*stmt.condition))) {
    std::visit(*this, *(stmt.body));
  }
}

void Interpreter::operator()(Return &stmt) {
  LiteralObject value = std::monostate{};

  if (stmt.value != nullptr) {
    value = evaluate(*(stmt.value));
  }

  throw new FuncReturn(value);
}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> &stmts) {
  try {
    for (std::shared_ptr<Stmt> &stmt : stmts) {
      std::visit(*this, *stmt);
    }
  } catch (RuntimeError *error) {
    errorReporter.runtimeError(error);
  }
}

LiteralObject Interpreter::evaluate(Expr &expr) {
  return std::visit(*this, expr);
}

void Interpreter::executeBlock(std::vector<std::shared_ptr<Stmt>> statements,
                               std::shared_ptr<Environment> environment) {
  std::shared_ptr<Environment> previous = this->environment;

  try {
    this->environment = environment;

    for (std::shared_ptr<Stmt> stmt : statements) {
      std::visit(*this, *stmt);
    }
    this->environment = previous;
  } catch (RuntimeError *error) {
    this->environment = previous;
    throw error;
  }
}

void Interpreter::resolve(std::shared_ptr<Expr> expr, int hops, size_t id) {
  locals.insert({id, hops});
}
