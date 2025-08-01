#pragma once

#include "token_type.hpp"
#include <string>
#include <variant>

using LiteralObject = std::variant<std::monostate, std::string, double, bool>;

class Token {
public:
  TokenType type;
  std::string lexeme;
  LiteralObject literal;
  int line;

  Token(TokenType type, std::string lexeme, LiteralObject literal, int line);

  std::string toString();
};
