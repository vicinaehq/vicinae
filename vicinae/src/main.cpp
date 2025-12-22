#include "cli/cli.hpp"
#include "config/config.hpp"
#include <QString>

int main(int argc, char **argv) {

  // std::cout << "final toml\n" << cfg.value().toToml() << std::endl;

  return CommandLineInterface::execute(argc, argv);
}
