#include "cli.hpp"
#include "cli/theme.hpp"
#include "daemon/ipc-client.hpp"
#include "utils.hpp"
#include "lib/CLI11.hpp"
#include "ui/dmenu-view/dmenu-view.hpp"
#include "vicinae.hpp"
#include "server.hpp"
#include <iostream>
#include <qdir.h>
#include <qobjectdefs.h>
#include <stdexcept>
#include "lib/rang.hpp"
#include <exception>
#include "vicinae.hpp"
#include "ext-clip/app.hpp"
#include "services/clipboard/ext/ext-clipboard-server.hpp"
#include "wlr-clip/app.hpp"
#include "services/clipboard/wlr/wlr-clipboard-server.hpp"
#include "version.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <iomanip>
#include <sstream>

class Formatter : public CLI::Formatter {
public:
  std::string make_help(const CLI::App *app, std::string name, CLI::AppFormatMode mode) const override {
    // ANSI color codes - we use these directly because rang doesn't work with stringstream
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
      out << BOLD << Omnicast::HEADLINE.toStdString() << RESET << "\n\n";
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
  void run(CLI::App *app) override {
    DaemonIpcClient client;
    client.launchApp(m_appId.c_str(), m_args, m_newInstance);
  }

private:
  std::string m_appId;
  std::vector<std::string> m_args;
  bool m_newInstance = false;
};

class ListAppsCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "list"; }
  std::string description() const override { return "List all tracked applications"; }

  void setup(CLI::App *app) override {
    app->alias("ls");
    app->add_flag("-j,--json", m_jsonOutput, "Output in JSON format");
    app->add_flag("--with-actions", m_withActions, "Include application actions/subactions");
  }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    auto apps = client.listApps(m_withActions);

    if (m_jsonOutput) {
      outputJson(apps);
    } else {
      outputText(apps);
    }
  }

private:
  bool m_jsonOutput = false;
  bool m_withActions = false;

  void outputText(const std::vector<proto::ext::daemon::AppInfo> &apps) {
    size_t maxIdLen = 2;
    size_t maxNameLen = 4;

    for (const auto &app : apps) {
      maxIdLen = std::max(maxIdLen, app.id().length());
      maxNameLen = std::max(maxNameLen, app.name().length());
    }

    std::cout << std::left << std::setw(maxIdLen + 2) << "ID" << std::setw(maxNameLen + 2) << "NAME"
              << "HIDDEN" << std::endl;

    std::cout << std::string(maxIdLen + 2, '-') << std::string(maxNameLen + 2, '-') << std::string(6, '-')
              << std::endl;

    for (const auto &app : apps) {
      std::cout << std::left << std::setw(maxIdLen + 2) << app.id() << std::setw(maxNameLen + 2) << app.name()
                << (app.hidden() ? "yes" : "no") << std::endl;
    }
  }

  void outputJson(const std::vector<proto::ext::daemon::AppInfo> &apps) {
    QJsonArray jsonApps;

    for (const auto &app : apps) {
      QJsonObject jsonApp;
      jsonApp["id"] = QString::fromStdString(app.id());
      jsonApp["name"] = QString::fromStdString(app.name());
      jsonApp["hidden"] = app.hidden();
      jsonApp["path"] = QString::fromStdString(app.path());
      jsonApp["description"] = QString::fromStdString(app.description());
      jsonApp["program"] = QString::fromStdString(app.program());
      jsonApp["is_terminal_app"] = app.is_terminal_app();
      jsonApp["icon_url"] = QString::fromStdString(app.icon_url());
      jsonApp["is_action"] = app.is_action();

      QJsonArray keywords;
      for (const auto &kw : app.keywords()) {
        keywords.append(QString::fromStdString(kw));
      }
      jsonApp["keywords"] = keywords;

      jsonApps.append(jsonApp);
    }

    QJsonDocument doc(jsonApps);
    std::cout << doc.toJson(QJsonDocument::Indented).toStdString();
  }
};

class AppCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "app"; }
  std::string description() const override { return "System application commands"; }
  void setup(CLI::App *app) override {}

public:
  AppCommand() {
    registerCommand<LaunchAppCommand>();
    registerCommand<ListAppsCommand>();
  }
};

class CliPing : public AbstractCommandLineCommand {
  std::string id() const override { return "ping"; }
  std::string description() const override { return "Ping the vicinae server"; }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    client.ping();
    std::cout << "Pinged successfully." << std::endl;
  }
};

class ToggleCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "toggle"; }
  std::string description() const override { return "Toggle the vicinae window"; }
  void setup(CLI::App *app) override { app->add_option("-q,--query", m_settings.query, "Set search query"); }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    client.toggle(m_settings);
  }

private:
  DaemonIpcClient::ToggleSettings m_settings;
};

class OpenCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "open"; }
  std::string description() const override { return "Open the vicinae window"; }
  void setup(CLI::App *app) override { app->add_option("-q,--query", m_settings.query, "Set search query"); }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    if (!client.open(m_settings)) { exit(1); }
  }

private:
  DaemonIpcClient::OpenSettings m_settings;
};

class CloseCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "close"; }
  std::string description() const override { return "Close the vicinae window"; }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    if (!client.close()) { exit(1); }
  }
};

class DMenuCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "dmenu"; }
  std::string description() const override { return "Render a list view from stdin"; }

  void setup(CLI::App *app) override {
    app->add_option("-n,--navigation-title", m_payload.navigationTitle, "Set the navigation title");
    app->add_option(
        "-s,--section-title", m_payload.sectionTitle,
        "Set the title of the main section. Use the {count} placeholder to render the current count.");
    app->add_option("-p,--placeholder", m_payload.placeholder, "Placeholder text to use in the search bar");
    app->add_option("-q,--query", m_payload.query, "Initial search query");
    app->add_flag("--no-section", m_payload.noSection, "Do not insert a section heading");
    app->add_flag("--no-quick-look", m_payload.noQuickLook,
                  "Do not show quick look if available for a given entry");
    app->add_flag("--no-metadata", m_payload.noMetadata, "Do not show metadata section in quick look");
  }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    client.connectOrThrow();
    m_payload.raw = Utils::slurp(std::cin);
    auto output = client.dmenu(m_payload);
    if (output.empty()) { exit(1); }
    std::cout << output << std::endl;
  }

private:
  DMenu::Payload m_payload;
};

class VersionCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "version"; }
  std::string description() const override { return "Show version and build information"; }
  void setup(CLI::App *app) override { app->alias("ver"); }

  void run(CLI::App *app) override {
    std::cout << "Version " << VICINAE_GIT_TAG << " (commit " << VICINAE_GIT_COMMIT_HASH << ")\n"
              << "Build: " << BUILD_INFO << "\n"
              << "Provenance: " << VICINAE_PROVENANCE << "\n";
  }
};

class DeeplinkCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "deeplink"; }
  std::string description() const override { return "Open a deeplink"; }

  void setup(CLI::App *app) override {
    app->alias("link");
    app->add_option("link", link, "The deeplink to open (see https://docs.vicinae.com/deeplinks)")
        ->required();
  }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    if (auto res = client.deeplink(QString(link.c_str())); !res) {
      throw std::runtime_error("Failed: " + res.error().toStdString());
    }
  }

private:
  std::string link;
};

int CommandLineApp::run(int ac, char **av) {
  CLI::App app(m_name);

  auto formatter = std::make_shared<Formatter>();
  app.formatter(formatter);

  for (const auto &cmd : m_cmds) {
    auto sub = app.add_subcommand(cmd->id(), cmd->description());
    cmd->prepare(sub);
    cmd->setup(sub);
    sub->callback([cmd = cmd.get(), sub]() { cmd->run(sub); });
  }

  if (ac == 1) {
    std::cout << app.help(av[0]);
    return 0;
  }

  if (ac == 2 && strcmp(av[1], ExtDataControlClipboardServer::ENTRYPOINT) == 0) {
    ExtClipman::instance()->start();
    return 0;
  }

  if (ac == 2 && strcmp(av[1], WlrClipboardServer::ENTRYPOINT) == 0) {
    Clipman::instance()->start();
    return 0;
  }

  // we still support direct deeplink usage
  // i.e vicinae vicinae://extensions/vicinae/clipboard/history
  if (ac == 2) {
    QString arg = av[1];
    // raycast:// or com.raycast:/
    auto pred = [&](const QString &scheme) { return arg.startsWith(scheme + ":/"); };
    bool hasScheme = std::ranges::any_of(Omnicast::APP_SCHEMES, pred);
    if (hasScheme) {
      char *subAv[] = {av[0], strdup("deeplink"), strdup(arg.toStdString().c_str()), nullptr};
      return run(3, subAv);
    }
  }

  app.require_subcommand();

  std::atexit([]() { std::cout << rang::style::reset; });

  try {
    app.parse(ac, av);
    return 0;
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
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> commands;
  CommandLineApp app(Omnicast::HEADLINE.toStdString());

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

  return app.run(ac, av);
}
