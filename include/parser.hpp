#pragma once

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <memory>
#include <vector>

class ParseError : public std::exception {};

class Parser {
private:
  std::vector<std::shared_ptr<Token>> tokens;
  int current = 0;

  bool match(std::vector<TokenType> types);
  bool check(TokenType type);
  std::shared_ptr<Token> advance();
  bool isAtEnd();
  std::shared_ptr<Token> peek();
  std::shared_ptr<Token> previous();

  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> comparison();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> unary();
  std::unique_ptr<Expr> primary();

  std::unique_ptr<Stmt> printStatement();
  std::unique_ptr<Stmt> expressionStatement();
  std::unique_ptr<Stmt> statement();

  std::shared_ptr<Token> consume(TokenType type, std::string message);
  ParseError error(Token token, std::string message);
  void synchronize();

public:
  Parser(std::vector<std::shared_ptr<Token>> tokens);
  std::vector<std::unique_ptr<Stmt>> parse();
};
