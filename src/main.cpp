#include "error_reporter.hpp"
#include "expr.hpp"
#include "interpreter.hpp"
#include "lox_callable.hpp"
#include "parser.hpp"
#include "resolver.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

ErrorReporter errorReporter{};
Interpreter interpreter{};

std::string readFile(std::string fileName) {
  std::ifstream file(fileName);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << fileName
              << ". Error: " << std::strerror(errno) << ".\n";
    return "";
  }

  std::string content{};
  std::string line{};

  while (std::getline(file, line)) {
    content += line + "\n";
  }

  if (file.bad()) {
    std::cerr << "Error reading file: " << fileName << ".\n";
    file.close();
    return "";
  }

  file.close();

  return content;
}

void run(std::string source) {
  std::shared_ptr<Scanner> scanner = std::make_shared<Scanner>(source);

  std::vector<std::shared_ptr<Token>> tokens = scanner->scanTokens();

  if (errorReporter.hadError)
    return;

  std::shared_ptr<Parser> parser = std::make_shared<Parser>(tokens);

  std::vector<std::shared_ptr<Stmt>> stmts = std::move(parser->parse());

  if (errorReporter.hadError)
    return;

  std::shared_ptr<Resolver> resolver = std::make_shared<Resolver>(interpreter);
  resolver->resolve(stmts);

  if (errorReporter.hadError)
    return;

  if (!stmts.empty())
    interpreter.interpret(stmts);
}

void runFile(std::string fileName) {
  std::string fileContent = readFile(fileName);

  run(fileContent);

  if (errorReporter.hadError || errorReporter.hadRuntimeError)
    std::exit(EXIT_FAILURE);
}

void runPrompt() {
  while (true) {
    std::cout << "> ";

    std::string line{};

    std::getline(std::cin, line);

    if (line == "\0")
      break;

    run(line);

    errorReporter.hadError = false;
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cerr << "Usage: CppLox [file]\n";
    return EXIT_FAILURE;
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
  }
}
