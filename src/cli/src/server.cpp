#include <array>
#include <cstring>
#include <ostream>
#include <unistd.h>
#include "server.hpp"
#include "common/common.hpp"
#include "common/CLI11.hpp"

namespace fs = std::filesystem;

void CliServerCommand::setup(CLI::App *app) {
  app->add_flag("--open", m_open, "Open the main window once the server is started");
  app->add_flag("--replace", m_replace, "Replace the currently running instance if there is one");
  app->add_option("--config", m_config, "Path to the main config file")
      ->default_val("settings.json")
      ->check([](const std::string &path) {
        std::error_code ec;
        if (!fs::is_regular_file(path, ec)) { return "not a valid file"; }
        return "";
      });
  app->add_flag("--no-extension-runtime", m_noExtensionRuntime,
                "Do not start the extension runtime node process. Typescript extensions will not run.");
}

bool CliServerCommand::run(CLI::App *app) {
  const auto path = vicinae::findHelperProgram("vicinae-server");

  if (!path) {
    std::println(std::cerr, "Could not find vicinae-server binary.");
    return false;
  }

  std::array<char *, 3> argv = {strdup("vicinae-server"), strdup("server"), nullptr};

  if (execv(path->c_str(), argv.data()) != 0) {
    std::println(std::cerr, "Failed to exec vicinae-server: {}", strerror(errno));
    return false;
  }

  return true;
}
