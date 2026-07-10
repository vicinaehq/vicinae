#include "cli.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int ac, char **av) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif
  return CommandLineInterface::execute(ac, av);
}
