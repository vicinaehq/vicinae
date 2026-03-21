#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen/typescript.hpp"

int main(int ac, char **av) {
  assert(ac == 3);

  std::ifstream ifs{av[1]};

  if (!ifs) {
    std::cerr << "could not read" << av[1] << std::endl;
    return 1;
  }

  std::ostringstream oss;

  oss << ifs.rdbuf();

  std::string data = oss.str();

  Lexer lexer{data};

  std::cout << data << std::endl;

  auto tree = Parser::parseTree(data);

  std::ofstream ofs(av[2]);

  ofs << codegenTypescript(tree);
}
