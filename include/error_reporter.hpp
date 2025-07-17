#pragma once

#include <string>

class ErrorReporter {
public:
  bool hadError{false};

  void reportError(int line, std::string message);
};
