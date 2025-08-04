#pragma once

#include "token_type.hpp"
#include <memory>
#include <string>
#include <variant>

class LoxCallable;

using LiteralObject = std::variant<std::monostate, std::string, double, bool,
                                   std::shared_ptr<LoxCallable>>;

struct StringifyLiteralVisitor {
  std::string operator()(std::monostate) const;

  std::string operator()(std::string str) const;

  std::string operator()(double num) const;

  std::string operator()(bool val) const;

  std::string operator()(std::shared_ptr<LoxCallable> callable) const;
};

struct TruthyLiteralVisitor {
  bool operator()(std::monostate) const;

  bool operator()(std::string str) const;

  bool operator()(double num) const;

  bool operator()(bool val) const;

  bool operator()(std::shared_ptr<LoxCallable> callable) const;
};

class Token {
public:
  TokenType type;
  std::string lexeme;
  LiteralObject literal;
  int line;

  Token(TokenType type, std::string lexeme, LiteralObject literal, int line);

  std::string toString() const;
};
