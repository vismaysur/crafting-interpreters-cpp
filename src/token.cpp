#include "token.hpp"
#include "lox_callable.hpp"
#include "token_type.hpp"
#include <iomanip>
#include <iostream>
#include <string>

Token::Token(TokenType type, std::string lexeme, LiteralObject literal,
             int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = std::move(literal);
  this->line = line;
}

std::string StringifyLiteralVisitor::operator()(std::monostate) const {
  return "nil";
}

std::string StringifyLiteralVisitor::operator()(std::string str) const {
  return str;
}

std::string StringifyLiteralVisitor::operator()(double num) const {
  return std::to_string(num);
}

std::string StringifyLiteralVisitor::operator()(bool val) const {
  return std::to_string(val);
}

std::string StringifyLiteralVisitor::operator()(
    std::shared_ptr<LoxCallable> callable) const {
  return callable->toString();
}

std::string StringifyLiteralVisitor::operator()(
    std::shared_ptr<LoxInstance> instance) const {
  return instance->toString();
}

bool TruthyLiteralVisitor::operator()(std::monostate) const { return false; }

bool TruthyLiteralVisitor::operator()(std::string str) const { return true; }

bool TruthyLiteralVisitor::operator()(double num) const { return true; }

bool TruthyLiteralVisitor::operator()(bool val) const { return val; }

bool TruthyLiteralVisitor::operator()(
    std::shared_ptr<LoxCallable> callable) const {
  return true;
}

bool TruthyLiteralVisitor::operator()(
    std::shared_ptr<LoxInstance> instance) const {
  return true;
}

std::string Token::toString() const {
  std::ostringstream oss{};

  oss << std::setw(10) << std::to_string(static_cast<int>(type));
  oss << std::setw(10) << lexeme;
  oss << std::setw(10) << std::visit(StringifyLiteralVisitor{}, literal);
  oss << std::setw(10) << line;

  return oss.str();
}
