#include "cli/cli.hpp"
#include "daemon/ipc-client.hpp"
#include "lib/rang.hpp"

class CliPing : public AbstractCommandLineCommand {
  std::string id() const override { return "ping"; }
  std::string description() const override { return "Ping the vicinae server"; }

  void run(CLI::App *app) override {
    DaemonIpcClient client;

    if (!client.connect()) {
      std::cout << "Failed to connect to the vicinae server." << rang::fg::reset;
      return;
    }

    std::cout << "Pinged successfully." << std::endl;
  }
};
