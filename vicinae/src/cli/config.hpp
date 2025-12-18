#pragma once
#include "cli/cli.hpp"
#include "config/template.hpp"
#include <stdexcept>

class TemplateConfigCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "template"; }
  std::string description() const override { return "Print out config template"; }
  void setup(CLI::App *app) override {}

  void run(CLI::App *app) override {
    std::cout << config::TEMPLATE << std::endl;
    return;
  }

private:
  std::optional<std::filesystem::path> m_path;
};

class DefaultConfigCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "default"; }
  std::string description() const override { return "Print out default config"; }
  void setup(CLI::App *app) override {}

  void run(CLI::App *app) override {
    auto file = QFile(":config.jsonc");

    if (!file.open(QIODevice::ReadOnly)) { throw std::runtime_error("Failed to open default config file!"); }

    std::cout << file.readAll().toStdString();
    return;
  }

private:
  std::optional<std::filesystem::path> m_path;
};

class ConfigCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "config"; }
  std::string description() const override { return "Config commands"; }
  void setup(CLI::App *app) override {}

  ConfigCommand() {
    registerCommand<TemplateConfigCommand>();
    registerCommand<DefaultConfigCommand>();
  }
};
