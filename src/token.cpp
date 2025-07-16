#include "token.hpp"
#include <string>

Token::Token(TokenType type, std::string lexeme, std::string literal,
             int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = literal;
  this->line = line;
}

std::string Token::toString() {
  return std::to_string(static_cast<int>(this->type)) + " " + this->lexeme +
         " " + this->literal;
}
