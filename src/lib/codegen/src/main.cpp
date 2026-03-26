#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen/typescript.hpp"
#include "codegen/glaze-qt.hpp"

int main(int ac, char **av) {
  assert(ac >= 3);

  std::ifstream ifs{av[1]};

  if (!ifs) {
    std::cerr << "could not read" << av[1] << std::endl;
    return 1;
  }

  std::ostringstream oss;

  oss << ifs.rdbuf();

  std::string data = oss.str();

  Lexer lexer{data};
  auto tree = Parser::parseTree(data);

  std::vector<std::unique_ptr<AbstractCodeGenerator>> generators;

  generators.emplace_back(std::make_unique<TypeScriptCodeGenerator>());
  generators.emplace_back(std::make_unique<GlazeQtGenerator>());

  for (const auto &gen : generators) {
    if (gen->name() == av[2]) {
      if (ac == 4) {
        std::ofstream{av[3]} << gen->generate(tree);
      } else {
        std::cout << gen->generate(tree);
      }
      return 0;
    }
  }

  std::cerr << "No generator with name " << av[2] << std::endl;
  return 1;
}
