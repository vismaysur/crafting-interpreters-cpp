#include "parser.hpp"
#include "error_reporter.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <algorithm>
#include <exception>
#include <memory>

extern ErrorReporter errorReporter;

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens) {
  this->tokens = tokens;
}

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
  return peek()->type == type;
}

std::shared_ptr<Token> Parser::advance() {
  if (!isAtEnd())
    current++;
  return previous();
}

bool Parser::isAtEnd() { return peek()->type == TokenType::_EOF; }

std::shared_ptr<Token> Parser::peek() { return tokens[current]; }

std::shared_ptr<Token> Parser::previous() { return tokens[current - 1]; }

std::unique_ptr<Expr> Parser::expression() { return equality(); }

std::unique_ptr<Expr> Parser::equality() {
  std::unique_ptr<Expr> expr = comparison();

  while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
    std::shared_ptr<Token> op = previous();
    std::unique_ptr<Expr> right = comparison();

    Binary binary(std::move(expr), *op, std::move(right));
    expr = std::make_unique<Expr>(std::move(binary));
  }

  return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
  std::unique_ptr<Expr> expr = term();

  while (match({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER,
                TokenType::GREATER_EQUAL})) {
    std::shared_ptr<Token> op = previous();
    std::unique_ptr<Expr> right = term();

    Binary binary(std::move(expr), *op, std::move(right));
    expr = std::make_unique<Expr>(std::move(binary));
  }

  return expr;
};

std::unique_ptr<Expr> Parser::term() {
  std::unique_ptr<Expr> expr = factor();

  while (match({TokenType::PLUS, TokenType::MINUS})) {
    std::shared_ptr<Token> op = previous();
    std::unique_ptr<Expr> right = factor();

    Binary binary(std::move(expr), *op, std::move(right));
    expr = std::make_unique<Expr>(std::move(binary));
  }

  return expr;
};

std::unique_ptr<Expr> Parser::factor() {
  std::unique_ptr<Expr> expr = unary();

  while (match({TokenType::STAR, TokenType::SLASH})) {
    std::shared_ptr<Token> op = previous();
    std::unique_ptr<Expr> right = unary();

    Binary binary(std::move(expr), *op, std::move(right));
    expr = std::make_unique<Expr>(std::move(binary));
  }

  return expr;
};

std::unique_ptr<Expr> Parser::unary() {
  while (match({TokenType::BANG, TokenType::MINUS})) {
    std::shared_ptr<Token> op = previous();
    std::unique_ptr<Expr> right = primary();

    Unary unary(*op, std::move(right));
    return std::make_unique<Expr>(std::move(unary));
  }

  return primary();
};

std::unique_ptr<Expr> Parser::primary() {
  if (match({TokenType::FALSE})) {
    Literal literal(false);
    return std::make_unique<Expr>(std::move(literal));
  }

  if (match({TokenType::TRUE})) {
    Literal literal(true);
    return std::make_unique<Expr>(std::move(literal));
  }

  if (match({TokenType::NIL})) {
    Literal literal(std::monostate{});
    return std::make_unique<Expr>(std::move(literal));
  }

  if (match({TokenType::NUMBER, TokenType::STRING})) {
    Literal literal(previous()->literal);
    return std::make_unique<Expr>(std::move(literal));
  }

  if (match({TokenType::LEFT_PAREN})) {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    Grouping grouping(std::move(expr));
    return std::make_unique<Expr>(std::move(grouping));
  }

  throw error(*peek(), "Expected an expression.");
}

std::shared_ptr<Token> Parser::consume(TokenType type, std::string message) {
  if (check(type))
    return advance();
  throw error(*peek(), message);
}

ParseError Parser::error(Token token, std::string message) {
  errorReporter.error(token, message);
  return ParseError{};
}

void Parser::synchronize() {
  advance();

  while (!isAtEnd()) {
    if (previous()->type == TokenType::SEMICOLON)
      return;

    switch (peek()->type) {
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

std::unique_ptr<Expr> Parser::parse() {
  try {
    return expression();
  } catch (const ParseError &error) {
    return nullptr;
  }
}
