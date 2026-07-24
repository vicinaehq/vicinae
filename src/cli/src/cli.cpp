#include <filesystem>
#include <glaze/core/opts.hpp>
#include <glaze/core/reflect.hpp>
#include <system_error>
#include "cli.hpp"
#include "config.hpp"
#include "fs.hpp"
#include "rang/rang.hpp"
#include "script.hpp"
#include "common/common.hpp"
#include "state.hpp"
#include "generated/version.h"
#include "theme.hpp"
#include "ipc-client.hpp"
#ifndef _WIN32
#include "server.hpp"
#endif

constexpr const auto HEADLINE = "A focused launcher for your desktop — native, fast, extensible";

class Formatter : public CLI::Formatter {
public:
  std::string make_help(const CLI::App *app, std::string name, CLI::AppFormatMode mode) const override {
    constexpr const auto BOLD = "\033[1m";
    constexpr const auto BRIGHT_GREEN = "\033[92m";
    constexpr const auto BLUE = "\033[34m";
    constexpr const auto CYAN = "\033[36m";
    constexpr const auto YELLOW = "\033[33m";
    constexpr const auto RESET = "\033[0m";
    std::stringstream out;

    // Show command description for subcommands, general headline for root
    if (app->get_parent() != nullptr && !app->get_description().empty()) {
      out << BOLD << app->get_description() << RESET << "\n\n";
    } else {
      out << BOLD << HEADLINE << RESET << "\n\n";
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

        out << "  " << YELLOW << sub->get_name() << RESET;

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

class LaunchAppCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "launch"; }
  std::string description() const override { return "Launch or focus an app from vicinae"; }

  void setup(CLI::App *app) override {
    app->add_option("app_id", m_appId, "The ID of the application");
    app->add_option("args", m_args, "Arguments to pass to the launched application");
    app->add_flag("--new", m_newInstance, "Always launch a new instance");
  }

  bool run(CLI::App *) override {
    const auto res = cli::IpcClient::connect().and_then([&](cli::IpcClient client) {
      return client.launchApp({.appId = m_appId, .args = m_args, .newInstance = m_newInstance});
    });

    if (!res) {
      std::println(std::cerr, "Failed to launch app: {}", res.error());
      return false;
    }

    if (auto title = res->focusedWindowTitle; !title.empty()) {
      std::println(std::cout,
                   "Focused existing window \"{}\"\nPass --new if you want to launch a new instance.", title);
    }
    return true;
  }

private:
  std::string m_appId;
  std::vector<std::string> m_args;
  bool m_newInstance = false;
};

class AppCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "app"; }
  std::string description() const override { return "System application commands"; }
  void setup(CLI::App *) override {}

public:
  AppCommand() { registerCommand<LaunchAppCommand>(); }
};

struct ListCommandsCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "ls"; }
  std::string description() const override { return "List loaded commands"; }

  void setup(CLI::App *app) override {
    app->alias("list");
    app->add_flag("--json,-j", m_json, "Output command list as json");
  }

  bool run(CLI::App *) override {
    auto res =
        cli::IpcClient::connect().and_then([](cli::IpcClient client) { return client.listCommands(); });

    if (!res) {
      std::println(std::cerr, "Failed to list commands: {}", res.error());
      return false;
    }

    if (m_json) {
      std::string buf;

      if (auto error =
              glz::write<glz::opts{.format = glz::JSON, .prettify = true}>(res.value().commands, buf)) {
        std::println(std::cerr, "Failed to serialize json: {}", glz::format_error(error));
        return false;
      }

      std::cout << buf << std::endl;
    } else {
      for (const auto &command : res->commands) {
        std::cout << command.id << "\n";
      }
    }

    return true;
  }

private:
  bool m_json;
};

class LaunchCommandCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "launch"; }
  std::string description() const override { return "Launch a command"; }

  void setup(CLI::App *app) override {
    app->add_option("entrypoint", m_entrypoint, "The command entrypoint ID to launch")->required();
    app->add_option("args", m_args, "Arguments to pass to the command");
    app->add_option("--cwd", m_cwd, "Working directory forwarded to the command");
    app->add_option("--query,-q", m_query, "");
  }

  bool run(CLI::App *) override {
    auto cwd = m_cwd;
    std::error_code ec;

    if (!cwd) {
      if (auto path = std::filesystem::current_path(ec); !ec) { cwd = path.generic_string(); }
    }

    auto res = cli::IpcClient::connect().and_then([&](cli::IpcClient client) {
      return client.launchCommand({.entrypoint = m_entrypoint, .args = m_args, .cwd = cwd, .query = m_query});
    });

    if (!res) {
      std::println(std::cerr, "Failed to launch command: {}", res.error());
      return false;
    }

    return true;
  }

private:
  std::string m_entrypoint;
  std::optional<std::string> m_cwd;
  std::optional<std::string> m_query;
  std::vector<std::string> m_args;
};

class CommandCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "cmd"; }
  std::string description() const override { return "Command utilities"; }
  void setup(CLI::App *app) override { app->alias("command"); }

public:
  CommandCommand() {
    registerCommand<ListCommandsCommand>();
    registerCommand<LaunchCommandCommand>();
  }
};

class CliPing : public AbstractCommandLineCommand {
  std::string id() const override { return "ping"; }
  std::string description() const override { return "Ping the vicinae server"; }

  bool run(CLI::App *) override {
    const auto res = cli::IpcClient::connect().and_then([](cli::IpcClient client) { return client.ping(); });

    if (!res) {
      std::println(std::cerr, "Failed to ping: {}", res.error());
      return false;
    }

    std::cout << "Pinged successfully." << std::endl;
    return true;
  }
};

class ToggleCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "toggle"; }
  std::string description() const override { return "Toggle the vicinae window"; }
  void setup(CLI::App *app) override { app->add_option("-q,--query", m_query, "Set search query"); }

  bool run(CLI::App *) override {
    std::vector<std::pair<std::string, std::string>> query;
    if (m_query) query = {{"fallbackText", m_query.value()}};

    if (auto res = cli::IpcClient::sendDeeplink("vicinae://toggle", query); !res) {
      std::println(std::cerr, "Failed to toggle: {}", res.error());
      return false;
    }
    return true;
  }

private:
  std::optional<std::string> m_query;
};

class OpenCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "open"; }
  std::string description() const override { return "Open the vicinae window"; }
  void setup(CLI::App *app) override { app->add_option("-q,--query", m_query, "Set search query"); }

  bool run(CLI::App *) override {
    std::vector<std::pair<std::string, std::string>> query;
    if (m_query) query = {{"fallbackText", m_query.value()}};

    if (auto res = cli::IpcClient::sendDeeplink("vicinae://open", query); !res) {
      std::println(std::cerr, "Failed to open: {}", res.error());
      return false;
    }
    return true;
  }

private:
  std::optional<std::string> m_query;
};

class CloseCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "close"; }
  std::string description() const override { return "Close the vicinae window"; }

  bool run(CLI::App *) override {
    if (auto res = cli::IpcClient::sendDeeplink("vicinae://close"); !res) {
      std::println(std::cerr, "Failed to close: {}", res.error());
      return false;
    }
    return true;
  }
};

class DMenuCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "dmenu"; }
  std::string description() const override { return "Render a list view from stdin"; }

  void setup(CLI::App *app) override {
    app->add_option("-n,--navigation-title", m_req.navigationTitle, "Set the navigation title");
    app->add_option(
        "-s,--section-title", m_req.sectionTitle,
        "Set the title of the main section. Use the {count} placeholder to render the current count.");
    app->add_option("-p,--placeholder", m_req.placeholder, "Placeholder text to use in the search bar");
    app->add_option("-q,--query", m_req.query, "Initial search query");
    app->add_option("-W,--width", m_req.width, "Window width in pixels");
    app->add_option("-H,--height", m_req.height, "Window height in pixels");
    app->add_flag("--no-section", m_req.noSection, "Do not insert a section heading");
    app->add_flag("--no-quick-look", m_req.noQuickLook,
                  "Do not show quick look if available for a given entry");
    app->add_flag("--no-metadata", m_req.noMetadata, "Do not show metadata section in quick look");
    app->add_flag("--no-footer", m_req.noFooter, "Hide the status bar footer");
    app->add_option("--format", m_req.format, "Output format")->check(CLI::IsMember({"text", "index"}));
  }

  bool run(CLI::App *) override {
    m_req.rawContent = vicinae::slurp(std::cin);

    const auto res =
        cli::IpcClient::connect().and_then([&](cli::IpcClient client) { return client.dmenu(m_req); });

    if (!res) {
      std::println(std::cerr, "Failed to invoke dmenu: {}", res.error());
      return false;
    }
    if (res->output.empty()) { return false; }

    std::println(std::cout, "{}", res->output);
    return true;
  }

private:
  ipc::DMenuRequest m_req;
};

class VersionCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "version"; }
  std::string description() const override { return "Show version and build information"; }
  void setup(CLI::App *app) override { app->alias("ver"); }

  bool run(CLI::App *) override {
    std::cout << "Version " << VICINAE_GIT_TAG << " (commit " << VICINAE_GIT_COMMIT_HASH << ")\n"
              << "Build: " << BUILD_INFO << "\n"
              << "Provenance: " << VICINAE_PROVENANCE << "\n";
    return true;
  }
};

class DeeplinkCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "deeplink"; }
  std::string description() const override { return "Open a deeplink"; }

  void setup(CLI::App *app) override {
    app->alias("link");
    app->add_option("link", link, "The deeplink to open. See https://docs.vicinae.com/deeplinks")->required();
  }

  bool run(CLI::App *) override {
    if (const auto result = cli::IpcClient::sendDeeplink(link); !result) {
      std::println(std::cerr, "Failed to execute deeplink: {}", result.error());
      return false;
    }
    return true;
  }

private:
  std::string link;
};

int CommandLineApp::run(int ac, char **av) {
  CLI::App app(m_name);

  auto formatter = std::make_shared<Formatter>();
  app.formatter(formatter);

  bool success = true;

  for (const auto &cmd : m_cmds) {
    auto sub = app.add_subcommand(cmd->id(), cmd->description());
    cmd->prepare(sub, &success);
    cmd->setup(sub);
    sub->callback([cmd = cmd.get(), sub, &success]() {
      const bool r = cmd->run(sub);
      success = r;
    });
  }

  if (ac == 1) {
    std::cout << app.help(av[0]);
    return 0;
  }

  // we still support direct deeplink usage
  // i.e vicinae vicinae://extensions/vicinae/clipboard/history
  if (ac == 2) {
    std::string arg = av[1];

    if (vicinae::isAppDeeplink(arg)) {
      if (auto res = cli::IpcClient::sendDeeplink(arg); !res) {
        std::println(std::cerr, "Deeplink execution failed: {}", res.error());
        return 1;
      }
      return 0;
    }
  }

  app.require_subcommand();

  std::atexit([]() { std::cout << rang::style::reset; });

  try {
    app.parse(ac, av);
    return success ? 0 : 1;
  } catch (const CLI::ParseError &e) {
    if (e.get_exit_code() != 0) { std::cerr << rang::fg::red; }
    return app.exit(e);
  } catch (const std::exception &except) {
    std::cerr << rang::fg::red;
    app.exit(CLI::Error("Exception", except.what()));
    return 1;
  }
}

int CommandLineInterface::execute(int ac, char **av) {
  CommandLineApp app(std::string{HEADLINE});

  app.registerCommand<VersionCommand>();
#ifndef _WIN32
  app.registerCommand<CliServerCommand>();
#endif
  app.registerCommand<CliPing>();
  app.registerCommand<ToggleCommand>();
  app.registerCommand<OpenCommand>();
  app.registerCommand<CloseCommand>();
  app.registerCommand<CommandCommand>();
  app.registerCommand<DeeplinkCommand>();
  app.registerCommand<DMenuCommand>();
  app.registerCommand<ThemeCommand>();
  app.registerCommand<FileSearchCommand>();
  app.registerCommand<AppCommand>();
  app.registerCommand<ConfigCommand>();
  app.registerCommand<ScriptCommand>();
  app.registerCommand<StateCommand>();

  return app.run(ac, av);
}
