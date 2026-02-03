#include "cli/cli.hpp"
#include "config/config.hpp"
#include <QString>
#include "script-command.hpp"

int main(int argc, char **argv) { return CommandLineInterface::execute(argc, argv); }
