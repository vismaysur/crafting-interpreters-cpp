#include "error_reporter.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

ErrorReporter errorReporter{};

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
  Scanner *scanner = new Scanner(source);
  std::vector<Token *> tokens = scanner->scanTokens();

  for (Token *token : tokens)
    std::cout << token->toString() << "\n";
}

void runFile(std::string fileName) {
  std::string fileContent = readFile(fileName);

  run(fileContent);

  if (errorReporter.hadError)
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
