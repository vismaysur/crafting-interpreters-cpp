#include "parser.hpp"
#include "error_reporter.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <algorithm>
#include <exception>
#include <memory>

extern ErrorReporter errorReporter;

Parser::Parser(std::vector<Token> tokens) { this->tokens = tokens; }

bool Parser::match(std::vector<TokenType> types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }

  return false;
}

bool Parser::check(TokenType type) {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

Token Parser::advance() {
  if (!isAtEnd())
    current++;
  return previous();
}

bool Parser::isAtEnd() { return peek().type == TokenType::_EOF; }

Token Parser::peek() { return tokens[current]; }

Token Parser::previous() { return tokens[current - 1]; }

Expr Parser::expression() { return equality(); }

Expr Parser::equality() {
  Expr expr = comparison();

  while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
    Token op = previous();
    Expr right = comparison();

    expr =
        Binary(std::make_shared<Expr>(expr), op, std::make_shared<Expr>(right));
  }

  return expr;
}

Expr Parser::comparison() {
  Expr expr = term();

  while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER,
                TokenType::GREATER_EQUAL})) {
    Token op = previous();
    Expr right = term();

    expr =
        Binary(std::make_shared<Expr>(expr), op, std::make_shared<Expr>(expr));
  }

  return expr;
};

Expr Parser::term() {
  Expr expr = factor();

  while (match({TokenType::PLUS, TokenType::MINUS})) {
    Token op = previous();
    Expr right = factor();

    expr =
        Binary(std::make_shared<Expr>(expr), op, std::make_shared<Expr>(expr));
  }

  return expr;
};

Expr Parser::factor() {
  Expr expr = unary();

  while (match({TokenType::STAR, TokenType::SLASH})) {
    Token op = previous();
    Expr right = unary();

    expr =
        Binary(std::make_shared<Expr>(expr), op, std::make_shared<Expr>(expr));
  }

  return expr;
};

Expr Parser::unary() {
  while (match({TokenType::BANG, TokenType::MINUS})) {
    Token op = previous();
    Expr right = unary();

    return Unary(op, std::make_shared<Expr>(right));
  }

  return primary();
};

Expr Parser::primary() {
  if (match({TokenType::FALSE}))
    return Literal(false);

  if (match({TokenType::TRUE}))
    return Literal(true);

  if (match({TokenType::NIL}))
    return Literal(std::monostate{});

  if (match({TokenType::NUMBER, TokenType::STRING}))
    return Literal(previous().literal);

  if (match({TokenType::LEFT_PAREN})) {
    Expr expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return Grouping(std::make_shared<Expr>(expr));
  }

  throw error(peek(), "Expected an expression.");
}

Token Parser::consume(TokenType type, std::string message) {
  if (check(type))
    return advance();
  throw error(peek(), message);
}

ParseError Parser::error(Token token, std::string message) {
  errorReporter.error(token, message);
  throw new ParseError{};
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous().type == TokenType::SEMICOLON)
      return;

    switch (peek().type) {
    case TokenType::CLASS:
    case TokenType::FUN:
    case TokenType::VAR:
    case TokenType::FOR:
    case TokenType::IF:
    case TokenType::WHILE:
    case TokenType::PRINT:
    case TokenType::RETURN:
      return;
    }

    advance();
  }
}

Expr Parser::parse() {
  try {
    return expression();
  } catch (ParseError error) {
    return std::monostate{};
  }
}
