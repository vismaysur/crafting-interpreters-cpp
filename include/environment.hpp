#include "expr.hpp"
#include "token.hpp"
#include <unordered_map>

class Environment {
private:
  std::unordered_map<std::string, LiteralObject> values{};

public:
  void define(std::string name, LiteralObject value);
  LiteralObject get(Token name);
};
