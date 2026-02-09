#include <stdexcept>
#include <string_view>
#include "theme.hpp"
#include "vicinae-ipc/client.hpp"
#include "THEME_TEMPLATE.hpp"

class SetCliThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "set"; }
  std::string description() const override { return "Set theme command"; }
  void setup(CLI::App *app) override { app->add_option("theme_id", m_path)->required(); }

  bool run(CLI::App *app) override {
    if (auto res = ipc::CliClient::deeplink(std::format("vicinae://theme/set/{}", m_path.c_str())); !res) {
      throw std::runtime_error("Failed to set theme: " + res.error());
    }
    return true;
  }

private:
  std::filesystem::path m_path;
};

class CheckThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "check"; }
  std::string description() const override { return "Check whether the target theme file is valid"; }
  void setup(CLI::App *app) override { app->add_option("file", m_path)->required(); }

  bool run(CLI::App *app) override {
    /*
ThemeParser parser;
auto res = parser.parse(m_path);

if (!res) { throw std::runtime_error("Theme is invalid: " + res.error()); }

for (const auto &diag : parser.diagnostics()) {
std::cout << rang::fg::yellow << "Warning: " << rang::fg::reset << diag << "\n";
}
std::cout << rang::fg::green << "Theme file is valid" << rang::fg::reset << "\n";
  */
    return true;
  }

private:
  std::filesystem::path m_path;
};

class TemplateThemeCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "template"; }
  std::string description() const override { return "Print out template"; }
  void setup(CLI::App *app) override { app->alias("tmpl"); }

  bool run(CLI::App *app) override {
    std::cout << THEME_TEMPLATE << std::endl;
    return true;
  }

private:
  std::optional<std::filesystem::path> m_path;
};

class ThemeSearchPathsCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "paths"; }
  std::string description() const override { return "Print the paths themes are searched at"; }

  bool run(CLI::App *app) override { return true; }
};

ThemeCommand::ThemeCommand() {
  // registerCommand<CheckThemeCommand>();
  // registerCommand<ThemeSearchPathsCommand>();
  registerCommand<TemplateThemeCommand>();
  registerCommand<SetCliThemeCommand>();
}
