#include "cli.hpp"
#include "config.hpp"
#include "lib/rang.hpp"
#include "script.hpp"
#include "common/common.hpp"
#include "version.h"
#include "theme.hpp"
#include "vicinae-ipc/client.hpp"
#include "server.hpp"
#include <filesystem>
#include <ranges>

constexpr const std::string_view HEADLINE = "A focused launcher for your desktop — native, fast, extensible";

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
      out << BOLD << HEADLINE << RESET << "\n\n";
    }

    std::string bin_name = name.empty() ? app->get_name() : name;
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
        int padding = 28 - printed_length;
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
        int padding = max_name_len - sub->get_name().length() + 8;
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

  bool run(CLI::App *app) override {
    const auto res = ipc::CliClient::oneshot<ipc::LaunchApp>(
        {.appId = m_appId, .args = m_args, .newInstance = m_newInstance});

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
  void setup(CLI::App *app) override {}

public:
  AppCommand() { registerCommand<LaunchAppCommand>(); }
};

class CliPing : public AbstractCommandLineCommand {
  std::string id() const override { return "ping"; }
  std::string description() const override { return "Ping the vicinae server"; }

  bool run(CLI::App *app) override {
    const auto res = ipc::CliClient::oneshot<ipc::Ping>({});

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

  bool run(CLI::App *app) override {
    ipc::CliClient::DeeplinkOptions opts;

    if (m_query) { opts.query = {{"fallbackText", m_query.value()}}; }

    if (auto res = ipc::CliClient::deeplink("vicinae://toggle", opts); !res) {
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

  bool run(CLI::App *app) override {
    ipc::CliClient::DeeplinkOptions opts;

    if (m_query) { opts.query = {{"fallbackText", m_query.value()}}; }

    if (auto res = ipc::CliClient::deeplink("vicinae://open", opts); !res) {
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

  bool run(CLI::App *app) override {
    if (auto res = ipc::CliClient::deeplink(std::format("vicinae://close")); !res) {
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
  }

  bool run(CLI::App *app) override {
    m_req.rawContent = vicinae::slurp(std::cin);

    const auto res = ipc::CliClient::oneshot<ipc::DMenu>(m_req);

    if (!res) {
      std::println(std::cerr, "Failed to invoke dmenu: {}", res.error());
      return false;
    }
    if (res->output.empty()) { return false; }

    std::println(std::cout, "{}", res->output);
    return true;
  }

private:
  ipc::DMenu::Request m_req;
};

class VersionCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "version"; }
  std::string description() const override { return "Show version and build information"; }
  void setup(CLI::App *app) override { app->alias("ver"); }

  bool run(CLI::App *app) override {
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

  bool run(CLI::App *app) override {
    if (const auto result = ipc::CliClient::deeplink(link); !result) {
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
      bool r = cmd->run(sub);
      success = r;
    });
  }

  if (ac == 1) {
    std::cout << app.help(av[0]);
    return 0;
  }

  if (std::string_view{av[1]} == "server") {
    const auto paths = vicinae::helperProgramCandidates("vicinae-server");
    std::error_code ec;

    for (const auto &path : paths) {
      if (std::filesystem::is_regular_file(path, ec)) {
        std::println(std::cerr, "Starting vicinae server executable at {}", path.c_str());
        if (execv(path.c_str(), av) != 0) {
          std::println(std::cerr, "Failed to exec vicinae-server: {}", strerror(errno));
          return 1;
        }
      }
    }

    const auto candidates = paths | std::views::transform([](auto &&t) { return t.string(); }) |
                            std::views::join_with('\n') | std::ranges::to<std::string>();

    std::println(std::cerr, "Could not find vicinae-server binary. Tried the following paths, in order:\n{}",
                 candidates);
    return 1;
  }

  // we still support direct deeplink usage
  // i.e vicinae vicinae://extensions/vicinae/clipboard/history
  if (ac == 2) {
    std::string arg = av[1];

    // raycast:// or com.raycast:/
    auto pred = [&](std::string_view scheme) { return arg.starts_with(std::string{scheme} + ":/"); };
    bool hasScheme = std::ranges::any_of(std::initializer_list{"vicinae", "raycast", "com.raycast"}, pred);

    if (hasScheme) {
      if (auto res = ipc::CliClient::deeplink(arg, {}); !res) {
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
  bool ignoreAppImageWarning = false;

  if (const char *p = getenv("IGNORE_APPIMAGE_WARNING"); p && std::string_view{p} == "1") {
    ignoreAppImageWarning = true;
  }

  /*
  if (Environment::isAppImage() && !ignoreAppImageWarning) {
    std::cerr
        << rang::fg::red
        << "Running Vicinae directly as an AppImage is not officially supported.\nThe AppImage version is "
           "intended to be installed using the script: https://docs.vicinae.com/install/script\nIf you "
           "REALLY want to do this, relaunch with IGNORE_APPIMAGE_WARNING=1. Please do not file bug reports
  " "if using it like this.\n"; return false;
  }
  */

  std::vector<std::unique_ptr<AbstractCommandLineCommand>> commands;
  CommandLineApp app(std::string{HEADLINE});

  app.registerCommand<VersionCommand>();
  app.registerCommand<CliServerCommand>();
  app.registerCommand<CliPing>();
  app.registerCommand<ToggleCommand>();
  app.registerCommand<OpenCommand>();
  app.registerCommand<CloseCommand>();
  app.registerCommand<DeeplinkCommand>();
  app.registerCommand<DMenuCommand>();
  app.registerCommand<ThemeCommand>();
  app.registerCommand<AppCommand>();
  app.registerCommand<ConfigCommand>();
  app.registerCommand<ScriptCommand>();

  return app.run(ac, av);
}
