#pragma once
#include "lib/CLI11.hpp"
#include "lib/rang.hpp"
#include <type_traits>

class AbstractCommandLineCommand {
public:
  virtual std::string id() const = 0;
  virtual std::string description() const { return std::string(); }

  virtual void setup(CLI::App *app) {}
  virtual void run(CLI::App *app) = 0;

  virtual ~AbstractCommandLineCommand() = default;
};

template <typename T>
concept CliCommandType = std::is_base_of_v<AbstractCommandLineCommand, T>;

class CommandLineApp {
public:
  CommandLineApp(const std::string &name) : m_name(name) {}

  template <CliCommandType T> void registerCommand() { m_cmds.emplace_back(std::make_unique<T>()); }

  int run(int ac, char **av) {
    CLI::App app(m_name);

    for (const auto &cmd : m_cmds) {
      auto sub = app.add_subcommand(cmd->id(), cmd->description());
      cmd->setup(sub);
      sub->callback([cmd = cmd.get(), sub]() { cmd->run(sub); });
    }

    if (ac == 1) {
      std::cout << app.help() << std::endl;
      return 0;
    }

    app.require_subcommand();

    std::atexit([]() { std::cout << rang::style::reset; });

    try {
      app.parse(ac, av);
      return 0;
    } catch (const CLI::ParseError &e) {
      if (e.get_exit_code() != 0) { std::cout << rang::fg::red; }
      return app.exit(e);
    }
  }

private:
  std::string m_name;
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> m_cmds;
};

class CommandLineInterface {
public:
  static bool execute(int ac, char **av);

private:
  static void launchServer(bool detach = false);
};
