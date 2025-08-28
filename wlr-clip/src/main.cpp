#include "app.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <iostream>
#include <iostream>
#include <ostream>
#include <sys/syscall.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

int main(int ac, char **av) {
  if (isatty(STDIN_FILENO)) {
    std::cerr << av[0]
              << " started directly from TTY. Start interacting with the clipboard and "
                 "selection events will show up here"
              << std::endl;
  }

  Clipman::instance()->start();
}
