#include "interpreter.hpp"
#include "error_reporter.hpp"
#include "expr.hpp"
#include "runtime_error.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <exception>
#include <iostream>
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

LiteralObject Interpreter::evaluate(Expr expr) const {
  return std::visit(*this, expr);
}

LiteralObject Interpreter::operator()(Assign assign) const {
  LiteralObject value = evaluate(assign.value);
  environment->assign(assign.name, value);
  return value;
}

LiteralObject Interpreter::operator()(Literal literal) const {
  return literal.value;
}

LiteralObject Interpreter::operator()(Grouping grouping) const {
  return evaluate(*grouping.expression);
}

LiteralObject Interpreter::operator()(Unary unary) const {
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

LiteralObject Interpreter::operator()(Binary binary) const {
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

LiteralObject Interpreter::operator()(Variable expr) const {
  return environment->get(expr.name);
}

void Interpreter::operator()(Print stmt) const {
  LiteralObject value = evaluate(stmt.expr);
  std::cout << std::visit(StringifyLiteralVisitor{}, value) << std::endl;
}

void Interpreter::operator()(Expression stmt) const { evaluate(stmt.expr); }

void Interpreter::operator()(Var stmt) const {
  LiteralObject value = std::monostate{};
  if (stmt.initializer != nullptr) {
    value = evaluate(stmt.initializer);
  }

  environment->define(stmt.name.lexeme, value);
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>> &stmts) {
  try {
    for (std::unique_ptr<Stmt> &stmt : stmts) {
      std::visit(*this, *stmt);
    }
  } catch (RuntimeError *error) {
    errorReporter.runtimeError(error);
  }
}
