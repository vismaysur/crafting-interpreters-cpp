#pragma once

#include "token.hpp"
#include "token_type.hpp"
#include <exception>
#include <stdexcept>
#include <string>

class RuntimeError : public std::runtime_error {
public:
  Token token{TokenType::NIL, "", std::monostate{}, 1};
  std::string message;

  RuntimeError(Token token, std::string message);
};
