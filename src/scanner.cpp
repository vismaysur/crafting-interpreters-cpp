#include "scanner.hpp"
#include "error_reporter.hpp"
#include "token_type.hpp"
#include <memory>
#include <variant>

extern ErrorReporter errorReporter;

Scanner::Scanner(std::string source) { this->source = source; }

std::vector<std::shared_ptr<Token>> Scanner::scanTokens() {
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  std::shared_ptr<Token> lastToken = std::shared_ptr<Token>(
      new Token(TokenType::_EOF, "", std::monostate{}, line));

  tokens.push_back(lastToken);

  return tokens;
}

bool Scanner::isAtEnd() { return current >= source.length(); }

void Scanner::scanToken() {
  char c = advance();

  switch (c) {
  case '(':
    addToken(TokenType::LEFT_PAREN);
    break;
  case ')':
    addToken(TokenType::RIGHT_PAREN);
    break;
  case '{':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '}':
    addToken(TokenType::RIGHT_BRACE);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case '*':
    addToken(TokenType::STAR);
    break;

  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;

  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;

  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;

  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;

  case '/':
    if (match('/')) {
      while (peek() != '\n' && !isAtEnd())
        advance();
    } else {
      addToken(TokenType::SLASH);
    }
    break;

  case ' ':
  case '\r':
  case '\t':
    break;

  case '\n':
    line++;
    break;

  case '"':
    consumeString();
    break;

  default:
    if (isDigit(c)) {
      consumeNumber();
    } else if (isAlpha(c)) {
      consumeIdentifier();
    } else {
      errorReporter.reportError(line, "", "Unexpected character");
    }
    break;
  }
}

char Scanner::advance() { return source[current++]; }

bool Scanner::match(char expected) {
  if (isAtEnd())
    return false;
  if (source[current] != expected)
    return false;

  current++;
  return true;
}

char Scanner::peek() {
  if (isAtEnd())
    return '\0';
  return source[current];
}

char Scanner::peekNext() {
  if (current + 1 >= source.length())
    return '\0';
  return source[current + 1];
}

void Scanner::addToken(TokenType type) { addToken(type, std::monostate{}); }

void Scanner::addToken(TokenType type, LiteralObject literal) {
  std::string text = source.substr(start, current - start);

  std::shared_ptr<Token> token =
      std::shared_ptr<Token>(new Token(type, text, literal, line));

  tokens.push_back(token);
}

void Scanner::consumeString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    advance();
  }

  if (isAtEnd()) {
    errorReporter.reportError(line, "", "Unterminated string.");
    return;
  }

  advance();

  std::string value = source.substr(start + 1, current - (start + 1) - 1);
  addToken(TokenType::STRING, value);
}

void Scanner::consumeNumber() {
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peekNext())) {
    advance();

    while (isDigit(peek()))
      advance();
  }

  addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
}

void Scanner::consumeIdentifier() {
  while (isAlphaNumeric(peek()))
    advance();

  std::string text = source.substr(start, current - start);

  TokenType type{TokenType::IDENTIFIER};

  if (keywords.find(text) != keywords.end()) {
    type = keywords[text];
  }

  if (type == TokenType::TRUE || type == TokenType::FALSE) {
    addToken(type, type == TokenType::TRUE ? true : false);
  }

  addToken(type);
}

bool Scanner::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(char c) { return c >= '0' && c <= '9'; }
