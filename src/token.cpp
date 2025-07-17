#include "token.hpp"
#include <string>

Token::Token(TokenType type, std::string lexeme, Literal literal, int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = literal;
  this->line = line;
}

struct LiteralVisitor {
  std::string operator()(std::monostate) const { return "nil"; }

  std::string operator()(std::string str) const { return str; }

  std::string operator()(double num) const { return std::to_string(num); }
};

std::string Token::toString() {
  return std::to_string(static_cast<int>(type)) + " " + lexeme + " " +
         std::visit(LiteralVisitor{}, literal);
}
