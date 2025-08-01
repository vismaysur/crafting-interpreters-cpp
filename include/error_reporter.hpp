#pragma once

#include "token.hpp"
#include <string>

class ErrorReporter {
public:
  bool hadError{false};

  void reportError(int line, std::string where, std::string message);

  void error(Token token, std::string message);
};
