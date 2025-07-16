#include <unordered_map>
#include <vector>

#include "token.hpp"

class Scanner {
private:
  std::string source;
  std::vector<Token *> tokens{};

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

  void addToken(TokenType type, std::string literal);

  void consumeString();

  void consumeNumber();

  void identifier();

  bool isAlpha();

  bool isAlphaNumeric();

  bool isAtEnd();

  bool isDigit(char c);

  bool match(char expected);

  char peek();

  char peekNext();

  void scanToken();

public:
  Scanner(std::string source);

  std::vector<Token *> scanTokens();
};
