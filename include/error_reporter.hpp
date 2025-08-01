#pragma once

#include "runtime_error.hpp"
#include "token.hpp"
#include <string>

class ErrorReporter {
public:
  bool hadError{false};
  bool hadRuntimeError{false};

  void reportError(int line, std::string where, std::string message);

  void error(Token token, std::string message);

  void runtimeError(RuntimeError error);
};
