#pragma once
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "codegen/typescript.hpp"
#include "codegen/glaze-qt.hpp"
#include "common/CLI11.hpp"

class AbstractCommandLineCommand {
public:
  virtual std::string id() const = 0;
  virtual std::string description() const { return std::string(); }

  template <typename T> void registerCommand() { m_cmds.emplace_back(std::make_unique<T>()); }

  void prepare(CLI::App *app, bool *result = nullptr) {
    for (const auto &cmd : m_cmds) {
      auto sub = app->add_subcommand(cmd->id(), cmd->description());
      cmd->prepare(sub, result);
      cmd->setup(sub);
      sub->callback([cmd = cmd.get(), sub, result]() {
        const bool r = cmd->run(sub);
        if (result) *result = r;
      });
    }

    //.if (!m_cmds.empty()) { app->require_subcommand(); }
  }

  virtual void setup(CLI::App *) {}

  virtual bool run(CLI::App *app) {
    if (!m_cmds.empty() && app->get_subcommands().empty()) { std::cout << app->help() << std::endl; }
    return true;
  }

  virtual ~AbstractCommandLineCommand() = default;

private:
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> m_cmds;
};

template <typename T>
concept CliCommandType = std::is_base_of_v<AbstractCommandLineCommand, T>;

class Formatter : public CLI::Formatter {
public:
  std::string make_help(const CLI::App *app, std::string name, CLI::AppFormatMode mode) const override {
    const std::string BOLD = "\033[1m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BLUE = "\033[34m";
    const std::string CYAN = "\033[36m";
    const std::string RESET = "\033[0m";
    std::stringstream out;

    // Show command description for subcommands, general headline for root
    if (app->get_parent() != nullptr && !app->get_description().empty()) {
      out << BOLD << app->get_description() << RESET << "\n\n";
    } else {
      out << BOLD << "json-rpc code gen" << RESET << "\n\n";
    }

    const std::string bin_name = name.empty() ? app->get_name() : name;
    out << BOLD << "Usage: " << RESET;

    if (app->get_parent() != nullptr) {
      out << bin_name << " [...flags] [...args]\n\n";
    } else {
      out << bin_name << " <command> [...flags] [...args]\n\n";
    }

    auto options = app->get_options();
    if (!options.empty()) {
      out << BOLD << "Options:\n" << RESET;
      for (const auto *opt : options) {
        if (opt->get_name().empty()) continue;

        out << "  " << BRIGHT_GREEN;

        // Format short and long names
        auto snames = opt->get_snames();
        auto lnames = opt->get_lnames();

        // Calculate actual printed length for alignment
        int printed_length = 0;
        if (!snames.empty()) {
          out << "-" << snames[0];
          printed_length += 1 + snames[0].length(); // "-" + short name
          if (!lnames.empty()) {
            out << ", --";
            printed_length += 4; // ", --"
          }
        } else if (!lnames.empty()) {
          out << "    --";
          printed_length += 6; // "    --"
        } else if (auto name = opt->get_name(); !name.empty()) {
          out << name;
          printed_length += name.length();
        }

        if (!lnames.empty()) {
          out << lnames[0];
          printed_length += lnames[0].length();
        }

        out << RESET;

        // Pad to align descriptions
        const int padding = 28 - printed_length;
        if (padding > 0) {
          out << std::string(padding, ' ');
        } else {
          out << "  ";
        }

        out << opt->get_description() << "\n";
      }
      out << "\n";
    }

    // Subcommands section
    auto subcommands = app->get_subcommands({});
    if (!subcommands.empty()) {
      out << BOLD << "Commands:\n" << RESET;

      // Find max command name length for alignment
      size_t max_name_len = 0;
      for (const auto *sub : subcommands) {
        if (sub->get_name().empty()) continue;
        max_name_len = std::max(max_name_len, sub->get_name().length());
      }

      for (const auto *sub : subcommands) {
        if (sub->get_name().empty()) continue;

        out << "  " << BLUE << sub->get_name() << RESET;

        // Pad to align descriptions
        const int padding = max_name_len - sub->get_name().length() + 8;
        if (padding > 0) {
          out << std::string(padding, ' ');
        } else {
          out << "  ";
        }

        out << sub->get_description() << "\n";
      }
    }

    if (mode == CLI::AppFormatMode::Normal && app->get_parent() == nullptr) {
      out << "\n";
      out << "Learn more about Vicinae:        " << BLUE << "https://vicinae.com/docs" << RESET << "\n";
      out << "Join our Discord community:      " << CYAN << "https://vicinae.com/discord" << RESET << "\n";
    }

    return out.str();
  }
};

class CommandLineApp {
public:
  CommandLineApp(std::string name) : m_name(std::move(name)) {}
  template <CliCommandType T> void registerCommand() { m_cmds.emplace_back(std::make_unique<T>()); }
  int run(int ac, char **av) {
    CLI::App app(m_name);

    auto formatter = std::make_shared<Formatter>();
    app.formatter(formatter);

    bool success = true;

    for (const auto &cmd : m_cmds) {
      auto sub = app.add_subcommand(cmd->id(), cmd->description());
      cmd->prepare(sub, &success);
      cmd->setup(sub);
      sub->callback([cmd = cmd.get(), sub, &success]() { success = cmd->run(sub); });
    }

    if (ac == 1) {
      std::cout << app.help(av[0]);
      return 0;
    }

    app.require_subcommand();

    std::atexit([]() {});

    try {
      app.parse(ac, av);
      return success ? 0 : 1;
    } catch (const CLI::ParseError &e) {
      if (e.get_exit_code() != 0) {}
      return app.exit(e);
    } catch (const std::exception &except) {
      app.exit(CLI::Error("Exception", except.what()));
      return 1;
    }
  }

private:
  std::string m_name;
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> m_cmds;
};

class CompileCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "compile"; }

  void setup(CLI::App *app) override {
    app->add_option("proto", m_proto)->required();
    app->add_option("--output,-o", m_out, "")->required();
    app->add_option("--client", m_clients, "");
    app->add_option("--server", m_servers, "");
  }

  bool run(CLI::App *app) override {
    std::ifstream ifs{m_proto};

    if (!ifs) {
      std::cerr << "could not read" << m_proto << std::endl;
      return false;
    }

    std::ostringstream oss;

    oss << ifs.rdbuf();

    std::string data = oss.str();

    Lexer lexer{data};
    auto result = Parser::parseTree(data);

    if (!result) {
      std::cerr << "Failed to parse " << m_proto << "\n" << result.error() << std::endl;
      return false;
    }

    auto tree = std::move(result).value();

    std::vector<std::unique_ptr<AbstractCodeGenerator>> generators;

    generators.emplace_back(std::make_unique<TypeScriptCodeGenerator>());
    generators.emplace_back(std::make_unique<GlazeQtGenerator>());

    for (const auto &client : m_clients) {
      for (const auto &gen : generators) {
        if (gen->name() == client) {
          std::ofstream{m_out} << gen->generateClient(tree);
          std::cout << "generated " << client << " client at " << m_out << std::endl;
          return true;
        }
      }
    }

    for (const auto &server : m_servers) {
      for (const auto &gen : generators) {
        if (gen->name() == server) {
          std::ofstream{m_out} << gen->generateServer(tree);
          std::cout << "generated " << server << " server at " << m_out << std::endl;
          return true;
        }
      }
    }

    return true;
  }

private:
  std::string m_proto;
  std::string m_out;
  std::vector<std::string> m_clients;
  std::vector<std::string> m_servers;
};

int main(int ac, char **av) {
  CommandLineApp cli{"figura"};

  cli.registerCommand<CompileCommand>();

  return cli.run(ac, av);
}
