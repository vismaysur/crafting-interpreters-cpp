#pragma once

#include "error_reporter.hpp"
#include "token.hpp"
#include <unordered_map>
#include <vector>

class Scanner {
private:
  std::string source;
  std::vector<Token *> tokens{};
  ErrorReporter &errorReporter;

  std::unordered_map<std::string, TokenType> keywords{
      {"and", TokenType::AND},       {"class", TokenType::CLASS},
      {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
      {"for", TokenType::FOR},       {"fun", TokenType::FUN},
      {"if", TokenType::IF},         {"nil", TokenType::NIL},
      {"or", TokenType::OR},         {"print", TokenType::PRINT},
      {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
      {"this", TokenType::THIS},     {"true", TokenType::TRUE},
      {"var", TokenType::VAR},       {"while", TokenType::WHILE}};

  int start{0};
  int current{0};
  int line{1};

  char advance();

  void addToken(TokenType type);

  void addToken(TokenType type, Literal literal);

  void consumeIdentifier();

  void consumeString();

  void consumeNumber();

  bool isAlpha(char c);

  bool isAlphaNumeric(char c);

  bool isAtEnd();

  bool isDigit(char c);

  bool match(char expected);

  char peek();

  char peekNext();

  void scanToken();

public:
  Scanner(std::string source, ErrorReporter &errorReporter);

  std::vector<Token *> scanTokens();
};
