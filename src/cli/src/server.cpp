#include <csignal>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <glaze/glaze.hpp>
#include "server.hpp"
#include "ipc-client.hpp"
#include "common/common.hpp"
#include "common/CLI11.hpp"

struct ServerLaunchOptions {
  bool open = false;
  bool noExtensionRuntime = false;
  std::string config;
};

void CliServerCommand::setup(CLI::App *app) {
  app->add_flag("--open", m_open, "Open the main window once the server is started");
  app->add_flag("--replace", m_replace, "Replace the currently running instance if there is one");
  app->add_option("--config", m_config, "Path to the main config file");
  app->add_flag("--no-extension-runtime", m_noExtensionRuntime,
                "Do not start the extension runtime node process. Typescript extensions will not run.");
}

bool CliServerCommand::run(CLI::App *) {
  auto pingRes = cli::IpcClient::connect().and_then([](cli::IpcClient c) { return c.ping(); });

  if (pingRes) {
    if (!m_replace) {
      std::println(std::cerr, "A server is already running (pid {}). Pass --replace to replace it.",
                   pingRes->pid);
      return false;
    }

    std::println(std::cerr, "Killing existing vicinae server (pid {})...", pingRes->pid);

    if (kill(pingRes->pid, SIGKILL) != 0) {
      std::println(std::cerr, "Failed to kill process: {}", strerror(errno));
      return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  const auto path = vicinae::findHelperProgram("vicinae-server");

  if (!path) {
    std::println(std::cerr, "Could not find vicinae-server binary.");
    return false;
  }

  std::string opts;
  glz::write_json(
      ServerLaunchOptions{.open = m_open, .noExtensionRuntime = m_noExtensionRuntime, .config = m_config},
      opts);

  char *argv[] = {strdup("vicinae-server"), strdup(opts.c_str()), nullptr};

  if (execv(path->c_str(), argv) != 0) {
    std::println(std::cerr, "Failed to exec vicinae-server: {}", strerror(errno));
    return false;
  }

  return true;
}
