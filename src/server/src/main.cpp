#include <cstdio>
#include <exception>
#include <iostream>
#include <glaze/glaze.hpp>
#include "common/common.hpp"
#include "server.hpp"

int main(int argc, char **argv) {
  vicinae::enableUtf8();

  std::set_terminate([] {
    if (auto e = std::current_exception()) {
      try {
        std::rethrow_exception(e);
      } catch (const std::exception &ex) {
        std::fprintf(stderr, "FATAL uncaught exception: %s\n", ex.what());
      } catch (...) { std::fprintf(stderr, "FATAL uncaught non-std exception\n"); }
    }
    std::fflush(stderr);
    std::abort();
  });

  ServerLaunchOptions opts;

  if (argc >= 2) {
    if (auto err = glz::read<glz::opts{.error_on_unknown_keys = false}>(opts, std::string_view{argv[1]})) {
      std::println(std::cerr, "Failed to parse launch options: {}", glz::format_error(err));
      return 1;
    }
  }

  return startServer(opts);
}
