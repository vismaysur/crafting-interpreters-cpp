#include "error_reporter.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <iostream>

void ErrorReporter::reportError(int line, std::string where,
                                std::string message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << "\n";
  hadError = true;
}

void ErrorReporter::error(Token token, std::string message) {
  if (token.type == TokenType::_EOF) {
    reportError(token.line, " at end", message);
  } else {
    reportError(token.line, " at '" + token.lexeme + "'", message);
  }
}

void ErrorReporter::runtimeError(RuntimeError error) {
  std::cout << error.message << "\n[line " << error.token.line << "]"
            << std::endl;
  hadRuntimeError = true;
}
