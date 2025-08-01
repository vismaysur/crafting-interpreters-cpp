#include "runtime_error.hpp"
#include <stdexcept>
#include <string>

RuntimeError::RuntimeError(Token token, std::string message)
    : std::runtime_error(message) {
  this->token = token;
}
