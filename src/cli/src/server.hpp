#include "cli.hpp"
#include <filesystem>

class CliServerCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "server"; }
  std::string description() const override { return "(Re)start the vicinae server"; }

  void setup(CLI::App *app) override;
  bool run(CLI::App *app) override;

private:
  bool m_open = false;
  bool m_replace = false;
  bool m_noExtensionRuntime = false;
  std::filesystem::path m_config;
};
