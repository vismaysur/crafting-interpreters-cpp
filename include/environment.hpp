#pragma once

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <unordered_map>

class Environment {
private:
  std::shared_ptr<Environment> enclosing;
  std::unordered_map<std::string, LiteralObject> values{};

public:
  Environment();
  Environment(std::shared_ptr<Environment> enclosing);

  void define(std::string name, LiteralObject value);
  void assign(Token name, LiteralObject value);
  LiteralObject get(Token name);
};
