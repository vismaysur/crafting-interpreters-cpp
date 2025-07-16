#include "scanner.hpp"

Scanner::Scanner(std::string source) { this->source = source; }

std::vector<Token *> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  tokens.push_back(new Token(TokenType::_EOF, "", nullptr, line));
  return tokens;
}
