#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

std::string readFile(std::string fileName) {
  std::ifstream file(fileName);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << fileName
              << ". Error: " << std::strerror(errno) << ".\n";
  }

  std::string content{};
  std::string line{};

  while (std::getline(file, line)) {
    content += line + "\n";
  }

  if (file.bad()) {
    std::cerr << "Error reading file: " << fileName << ".\n";
  }

  file.close();

  return content;
}

void run(std::string source) {}

void runFile(std::string fileName) {
  std::string fileContent = readFile(fileName);

  run(fileContent);
}

void runPrompt() {
  while (true) {
    std::cout << "> ";

    std::string line{};

    std::getline(std::cin, line);

    if (line == "\0")
      break;

    run(line);
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cerr << "Usage: cpplox [file]\n";
    return EXIT_FAILURE;
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
  }
}
