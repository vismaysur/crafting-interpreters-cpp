#pragma once

#include "interpreter.hpp"
#include "token.hpp"
#include <type_traits>
#include <vector>

class LoxCallable {
public:
  virtual int arity() = 0;
  virtual LiteralObject call(Interpreter interpreter,
                             std::vector<LiteralObject> args) = 0;
  virtual std::string toString() const = 0;
};

class ClockFunc : public LoxCallable {
public:
  int arity() override;
  LiteralObject call(Interpreter interpreter,
                     std::vector<LiteralObject> args) override;
  std::string toString() const override;
};

class LoxFunc : public LoxCallable {
private:
  std::shared_ptr<Func> funcDeclaration;
  std::shared_ptr<Environment> closure;

public:
  LoxFunc(std::shared_ptr<Func> funcDeclaration,
          std::shared_ptr<Environment> closure);

  int arity() override;
  LiteralObject call(Interpreter interpreter,
                     std::vector<LiteralObject> args) override;
  std::string toString() const override;
};
