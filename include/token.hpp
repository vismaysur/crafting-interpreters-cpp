#pragma once

#include "token_type.hpp"
#include <string>
#include <variant>

using Literal = std::variant<std::monostate, std::string, double>;

class Token {
private:
  TokenType type;
  std::string lexeme;
  Literal literal;
  int line;

public:
  Token(TokenType type, std::string lexeme, Literal literal, int line);

  std::string toString();
};
