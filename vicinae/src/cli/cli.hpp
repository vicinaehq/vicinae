#pragma once
#include "lib/CLI11.hpp"

class AbstractCommandLineCommand {
public:
  virtual std::string id() const = 0;
  virtual std::string description() const { return std::string(); }

  template <typename T> void registerCommand() { m_cmds.emplace_back(std::make_unique<T>()); }

  void prepare(CLI::App *app) {
    for (const auto &cmd : m_cmds) {
      auto sub = app->add_subcommand(cmd->id(), cmd->description());
      cmd->prepare(sub);
      cmd->setup(sub);
      sub->callback([cmd = cmd.get(), sub]() { cmd->run(sub); });
    }

    //.if (!m_cmds.empty()) { app->require_subcommand(); }
  }

  virtual void setup(CLI::App *app) {}

  virtual void run(CLI::App *app) {
    if (!m_cmds.empty() && app->get_subcommands().empty()) { std::cout << app->help() << std::endl; }
  }

  virtual ~AbstractCommandLineCommand() = default;

private:
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> m_cmds;
};

template <typename T>
concept CliCommandType = std::is_base_of_v<AbstractCommandLineCommand, T>;

class CommandLineApp {
public:
  CommandLineApp(const std::string &name) : m_name(name) {}
  template <CliCommandType T> void registerCommand() { m_cmds.emplace_back(std::make_unique<T>()); }
  int run(int ac, char **av);

private:
  std::string m_name;
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> m_cmds;
};

class CommandLineInterface {
public:
  static int execute(int ac, char **av);

private:
  static void launchServer(bool detach = false);
};
