#include "cli.hpp"
#include "daemon/ipc-client.hpp"
#include "lib/rang.hpp"
#include "theme/theme-db.hpp"
#include "utils.hpp"
#include "lib/CLI11.hpp"
#include "ui/dmenu-view/dmenu-view.hpp"
#include "vicinae.hpp"
#include "server.hpp"
#include <fstream>
#include <iostream>
#include <qdir.h>
#include <qobjectdefs.h>
#include <stdexcept>

class DescribeThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "describe"; }
  std::string description() const override { return "Print out the fully derived theme file"; }
  void setup(CLI::App *app) override {
    app->alias("desc");
    app->add_option("file", m_path)->required();
    app->add_flag("-j,--json", m_json,
                  "Output theme as json. Themes placed in the theme directories must only be TOML.");
  }

  void run(CLI::App *app) override {
    auto res = ThemeFile::fromFile(m_path);
    if (!res) { throw std::runtime_error("Theme is invalid: " + res.error().toStdString()); }

    if (m_json) {
      std::cout << res->toJson().toJson(QJsonDocument::JsonFormat::Indented).toStdString() << std::endl;
      return;
    }

    std::cout << res->toToml() << std::endl;
  }

private:
  std::filesystem::path m_path;
  bool m_json = false;
};

class CheckThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "check"; }
  std::string description() const override { return "Check whether the target theme file is valid"; }
  void setup(CLI::App *app) override { app->add_option("file", m_path)->required(); }

  void run(CLI::App *app) override {
    auto res = ThemeFile::fromFile(m_path);
    if (!res) { throw std::runtime_error("Theme is invalid: " + res.error().toStdString()); }
    std::cout << rang::fg::green << "Theme file is valid" << rang::fg::reset << "\n";
  }

private:
  std::filesystem::path m_path;
};

class TemplateThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "template"; }
  std::string description() const override { return "Print out template"; }
  void setup(CLI::App *app) override {
    app->alias("tmpl");
    app->add_option("-o,--output", m_path);
  }

  void run(CLI::App *app) override {
    std::cout << ThemeFile::vicinaeDark().toToml() << std::endl;
    return;
  }

private:
  std::optional<std::filesystem::path> m_path;
};

class ThemeSearchPathsCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "paths"; }
  std::string description() const override { return "Print the paths themes are searched at"; }

  void run(CLI::App *app) override {
    for (const auto &path : ThemeDatabase().searchPaths()) {
      std::cout << path << "\n";
    }
  }
};

class ThemeCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "theme"; }
  std::string description() const override { return "Theme-related commands"; }
  void setup(CLI::App *app) override { app->alias("th"); }

  ThemeCommand() {
    registerCommand<CheckThemeCommand>();
    registerCommand<ThemeSearchPathsCommand>();
    registerCommand<TemplateThemeCommand>();
    registerCommand<DescribeThemeCommand>();
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

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    client.toggle();
  }
};

class DMenuCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "dmenu"; }
  std::string description() const override { return "Render a list view from stdin"; }

  void setup(CLI::App *app) override {
    app->add_option("-n,--navigation-title", m_navigationTitle, "Set the navigation title");
    app->add_option(
        "-s,--section-title", m_sectionTitle,
        "Set the title of the main section. Use the {count} placeholder to render the current count.");
    app->add_option("-p,--placeholder", m_placeholder, "Placeholder text to use in the search bar");
    app->add_flag("--no-section", m_noSection, "Do not insert a section heading");
  }

  void run(CLI::App *app) override {
    DMenuListView::DmenuPayload payload;
    DaemonIpcClient client;

    client.connectOrThrow();
    payload.navigationTitle = m_navigationTitle;
    payload.placeholder = m_placeholder;
    payload.sectionTitle = m_sectionTitle;
    payload.noSection = m_noSection;
    payload.raw = Utils::slurp(std::cin);

    auto output = client.dmenu(payload);

    if (output.empty()) { exit(1); }

    std::cout << output << std::endl;
  }

private:
  std::string m_navigationTitle;
  std::string m_placeholder;
  std::string m_sectionTitle;
  bool m_noSection = false;
};

class VersionCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "version"; }
  std::string description() const override { return "Show version and build information"; }

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
    app->add_option("link", link, "The deeplink to open (see https://docs.vicinae.com/deeplinks)")
        ->required();
  }

  void run(CLI::App *app) override {
    DaemonIpcClient client;
    client.sendDeeplink(QString(link.c_str()));
  }

private:
  std::string link;
};

int CommandLineInterface::execute(int ac, char **av) {
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> commands;
  CommandLineApp app(Omnicast::HEADLINE.toStdString());

  app.registerCommand<VersionCommand>();
  app.registerCommand<CliServerCommand>();
  app.registerCommand<CliPing>();
  app.registerCommand<ToggleCommand>();
  app.registerCommand<DeeplinkCommand>();
  app.registerCommand<DMenuCommand>();
  app.registerCommand<ThemeCommand>();

  return app.run(ac, av);
}
