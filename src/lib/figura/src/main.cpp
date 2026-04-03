#include "cli.hpp"
#include "compile.hpp"

int main(int ac, char **av) {
  CommandLineApp cli{"figura"};

  cli.registerCommand<CompileCommand>();

  return cli.run(ac, av);
}
