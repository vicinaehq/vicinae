#include "cli.hpp"
#include "daemon/ipc-client.hpp"
#include "lib/CLI11.hpp"
#include "lib/rang.hpp"
#include "vicinae.hpp"
#include "ping.hpp"
#include "server.hpp"
#include <qobjectdefs.h>

class ToggleCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "toggle"; }
  std::string description() const override { return "Toggle the vicinae window"; }

  void run(CLI::App *app) override {
    DaemonIpcClient client;

    client.connect();
    client.toggle();
  }
};

class DmenuCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "dmenu"; }
  std::string description() const override { return "Render a list view from stdin"; }

  void setup(CLI::App *app) override {
    app->add_option("-t,--title", m_navigationTitle, "Set the navigation title");
  }

  void run(CLI::App *app) override {
    DaemonIpcClient client;

    client.connect();
  }

private:
  std::string m_navigationTitle;
};

class VersionCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "version"; }
  std::string description() const override { return "Show version and build information"; }

  void run(CLI::App *app) override {
    std::cout << "Version " << VICINAE_GIT_TAG << " (commit " << VICINAE_GIT_COMMIT_HASH << ")\n"
              << "Build: " << BUILD_INFO << "\n";
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
    client.connect();
    client.passUrl(QString(link.c_str()));
  }

private:
  std::string link;
};

bool CommandLineInterface::execute(int ac, char **av) {
  std::vector<std::unique_ptr<AbstractCommandLineCommand>> commands;
  CommandLineApp app(Omnicast::HEADLINE.toStdString());

  app.registerCommand<VersionCommand>();
  app.registerCommand<CliServerCommand>();
  app.registerCommand<CliPing>();
  app.registerCommand<ToggleCommand>();
  app.registerCommand<DeeplinkCommand>();
  app.registerCommand<DmenuCommand>();

  return app.run(ac, av) == 0;
}
