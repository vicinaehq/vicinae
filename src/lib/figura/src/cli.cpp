#include "cli.hpp"

std::string Formatter::make_help(const CLI::App *app, std::string name, CLI::AppFormatMode mode) const {
  const std::string BOLD = "\033[1m";
  const std::string BRIGHT_GREEN = "\033[92m";
  const std::string BLUE = "\033[34m";
  const std::string CYAN = "\033[36m";
  const std::string RESET = "\033[0m";
  std::stringstream out;

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

      auto snames = opt->get_snames();
      auto lnames = opt->get_lnames();

      int printed_length = 0;
      if (!snames.empty()) {
        out << "-" << snames[0];
        printed_length += 1 + snames[0].length();
        if (!lnames.empty()) {
          out << ", --";
          printed_length += 4;
        }
      } else if (!lnames.empty()) {
        out << "    --";
        printed_length += 6;
      } else if (auto name = opt->get_name(); !name.empty()) {
        out << name;
        printed_length += name.length();
      }

      if (!lnames.empty()) {
        out << lnames[0];
        printed_length += lnames[0].length();
      }

      out << RESET;

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

  auto subcommands = app->get_subcommands({});
  if (!subcommands.empty()) {
    out << BOLD << "Commands:\n" << RESET;

    size_t max_name_len = 0;
    for (const auto *sub : subcommands) {
      if (sub->get_name().empty()) continue;
      max_name_len = std::max(max_name_len, sub->get_name().length());
    }

    for (const auto *sub : subcommands) {
      if (sub->get_name().empty()) continue;

      out << "  " << BLUE << sub->get_name() << RESET;

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

int CommandLineApp::run(int ac, char **av) {
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
