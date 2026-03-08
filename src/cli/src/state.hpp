#pragma once
#include "cli.hpp"
#include "vicinae-ipc/client.hpp"
#include "vicinae-ipc/ipc.hpp"

class StateCommand : public AbstractCommandLineCommand {
  class OpenCommand : public AbstractCommandLineCommand {
    std::string id() const override { return "open"; }
    std::string description() const override { return "Exit with code 0 if window is open"; }
    bool run(CLI::App *app) override {
      auto const res = ipc::CliClient::oneshot<ipc::Describe>({});
      if (!res) { throw std::runtime_error("Failed to query open state: " + res.error()); }
      exit(!res->open);
    }
  };

public:
  std::string id() const override { return "state"; }
  std::string description() const override { return "Query vicinae state"; }
  void setup(CLI::App *app) override {}

  StateCommand() { registerCommand<OpenCommand>(); }
};
