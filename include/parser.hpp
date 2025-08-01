#include "expr.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <vector>

class ParseError : public std::exception {};

class Parser {
private:
  std::vector<Token> tokens;
  int current = 0;

  bool match(std::vector<TokenType> types);
  bool check(TokenType type);
  Token advance();
  bool isAtEnd();
  Token peek();
  Token previous();

  Expr expression();
  Expr equality();
  Expr comparison();
  Expr term();
  Expr factor();
  Expr unary();
  Expr primary();

  Token consume(TokenType type, std::string message);
  ParseError error(Token token, std::string message);
  void synchronize();

  Expr parse();

public:
  Parser(std::vector<Token> tokens);
};
