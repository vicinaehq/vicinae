#include <unistd.h>
#include <iostream>
#include <glaze/glaze.hpp>
#include "cli/server.hpp"

int main(int argc, char **argv) {
  ServerLaunchOptions opts;

  if (argc >= 2) {
    if (auto err = glz::read<glz::opts{.error_on_unknown_keys = false}>(opts, std::string_view{argv[1]})) {
      std::println(std::cerr, "Failed to parse launch options: {}", glz::format_error(err));
      return 1;
    }
  }

  return startServer(opts);
}
