#include "error_reporter.hpp"
#include <iostream>

void ErrorReporter::reportError(int line, std::string message) {
  std::cout << "[line " << line << "] Error: " << message << "\n";
  hadError = true;
}
