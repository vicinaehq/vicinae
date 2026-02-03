#pragma once
#include "cli.hpp"
#include "default-config.hpp"
#include <iostream>

class DefaultConfigCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "default"; }
  std::string description() const override { return "Print out default config"; }
  void setup(CLI::App *app) override {}

  bool run(CLI::App *app) override {
    std::cout << DEFAULT_CONFIG;
    return true;
  }
};

class ConfigCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "config"; }
  std::string description() const override { return "Config commands"; }
  void setup(CLI::App *app) override {}

  ConfigCommand() { registerCommand<DefaultConfigCommand>(); }
};
