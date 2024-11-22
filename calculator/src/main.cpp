#include "calculator.hpp"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  Parser parser;

  if (argc == 1) {
    std::string line;

    while (std::getline(std::cin, line)) {
      parser.evaluate(line);
    }

    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    parser.evaluate(argv[i]);
  }
}
