#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/generic.hpp>
#include <glaze/json.hpp>
#include <istream>
#include <ostream>
#include <string>
#include <iostream>
#include <glaze/glaze.hpp>
#include <unistd.h>

static std::optional<std::string> readMessage(std::istream &ifs, std::string &data) {
  uint32_t length = 0;

  ifs.read(reinterpret_cast<char *>(&length), sizeof(length));
  data.resize(length);
  ifs.read(data.data(), data.size());

  if (ifs.eof() && data.size() == 0) return {};

  return data;
}

int main() {
  if (isatty(STDIN_FILENO)) {
    std::println(std::cerr,
                 "This executable is designed to be used for native messaging, not to be run manually.");
    return 1;
  }

  std::string buf;

  while (const auto message = readMessage(std::cin, buf)) {
    glz::generic json{};

    if (auto error = glz::read_json(json, message.value())) {
      std::println(std::cerr, "Failed to parse json: {}", glz::format_error(error));
      continue;
    }

    std::println(std::cerr, "Processing message: {}", message.value());
  }

  std::println(std::cerr, "Got EOF, exiting...");
}
