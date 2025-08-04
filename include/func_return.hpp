#pragma once

#include "token.hpp"
#include <stdexcept>

class FuncReturn : public std::runtime_error {
public:
  LiteralObject value;

  FuncReturn(LiteralObject value);
};
