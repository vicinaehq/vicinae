#include "cli.hpp"
#include "common/common.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int ac, char **av) {
  vicinae::enableUtf8();
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif
  return CommandLineInterface::execute(ac, av);
}
