#pragma once

#include "token_type.hpp"
#include <string>
#include <variant>

using LiteralObject = std::variant<std::monostate, std::string, double, bool>;

struct StringifyLiteralVisitor {
  std::string operator()(std::monostate) const;

  std::string operator()(std::string str) const;

  std::string operator()(double num) const;

  std::string operator()(bool val) const;
};

struct TruthyLiteralVisitor {
  bool operator()(std::monostate) const;

  bool operator()(std::string str) const;

  bool operator()(double num) const;

  bool operator()(bool val) const;
};

class Token {
public:
  TokenType type;
  std::string lexeme;
  LiteralObject literal;
  int line;

  Token(TokenType type, std::string lexeme, LiteralObject literal, int line);

  std::string toString();
};
