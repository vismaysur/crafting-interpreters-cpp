#include "parser.hpp"
#include "error_reporter.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "token_type.hpp"
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <variant>

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

std::unique_ptr<Expr> Parser::expression() { return assignment(); }

std::unique_ptr<Expr> Parser::assignment() {
  std::unique_ptr<Expr> expr = equality();

  if (match({TokenType::EQUAL})) {
    std::shared_ptr<Token> equals = previous();
    std::unique_ptr<Expr> value = assignment();

    if (std::holds_alternative<Variable>(*expr)) {
      Variable variable = std::get<Variable>(*expr);
      Assign assign(variable.name, std::move(value));
      return std::make_unique<Expr>(assign);
    }

    error(*equals, "Invalid assignment target.");
  }

  return expr;
}

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

  if (match({TokenType::IDENTIFIER})) {
    std::shared_ptr<Token> name = previous();
    Variable variable(*name);
    return std::make_unique<Expr>(std::move(variable));
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

std::unique_ptr<Stmt> Parser::block() {
  std::vector<std::shared_ptr<Stmt>> statements{};

  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");

  Block block(std::move(statements));

  return std::make_unique<Stmt>(block);
}

std::unique_ptr<Stmt> Parser::printStatement() {
  std::unique_ptr<Expr> expr = expression();

  consume(TokenType::SEMICOLON, "Expected ';' after print statement.");

  Print print(std::move(expr));

  return std::make_unique<Stmt>(print);
}

std::unique_ptr<Stmt> Parser::ifStatement() {
  consume(TokenType::LEFT_PAREN, "Expected '(' after if.");
  std::unique_ptr<Expr> condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");

  std::unique_ptr<Stmt> thenBranch = statement();
  std::unique_ptr<Stmt> elseBranch = nullptr;
  if (match({TokenType::ELSE}))
    elseBranch = statement();

  If ifStmt(std::move(condition), std::move(thenBranch), std::move(elseBranch));

  return std::make_unique<Stmt>(ifStmt);
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
  std::unique_ptr<Expr> expr = expression();

  consume(TokenType::SEMICOLON, "Expected ';' after expression.");

  Expression expression(std::move(expr));

  return std::make_unique<Stmt>(expression);
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match({TokenType::IF}))
    return ifStatement();

  if (match({TokenType::LEFT_BRACE}))
    return block();

  if (match({TokenType::PRINT}))
    return printStatement();

  return expressionStatement();
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
  std::shared_ptr<Token> token =
      consume(TokenType::IDENTIFIER, "Expected variable name.");

  std::unique_ptr<Expr> initializer = nullptr;

  if (match({TokenType::EQUAL})) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");

  Var var(*token, std::move(initializer));

  return std::make_unique<Stmt>(var);
}

std::unique_ptr<Stmt> Parser::declaration() {
  try {
    if (match({TokenType::VAR})) {
      return varDeclaration();
    }

    return statement();
  } catch (const ParseError &error) {
    synchronize();

    return nullptr;
  }
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  std::vector<std::unique_ptr<Stmt>> statements{};

  while (!isAtEnd()) {
    std::unique_ptr<Stmt> stmt = declaration();

    if (stmt)
      statements.push_back(std::move(stmt));
  }

  return statements;
}
