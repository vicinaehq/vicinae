#include "calculator.hpp"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  Parser parser;

  if (argc == 1) {
    std::string line;

    while (std::getline(std::cin, line)) {
      std::cout << parser.evaluate(line) << std::endl;
    }

    return 0;
  }

  for (int i = 1; i < argc; ++i) {
    auto res = parser.evaluate(argv[i]);

    if (!res) {
      std::cout << "computation error " << res.error().what() << std::endl;
      return 0;
    }

    std::cout << res.value() << std::endl;
  }
}
