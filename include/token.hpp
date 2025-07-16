#include <string>

#include "token_type.hpp"

class Token {
private:
  TokenType type;
  std::string lexeme;
  std::string literal;
  int line;

public:
  Token(TokenType type, std::string lexeme, std::string literal, int line);

  std::string toString();
};
