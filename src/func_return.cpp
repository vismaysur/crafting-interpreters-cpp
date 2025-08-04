#include "func_return.hpp"
#include <exception>

FuncReturn::FuncReturn(LiteralObject value)
    : std::runtime_error(""), value(value) {}
