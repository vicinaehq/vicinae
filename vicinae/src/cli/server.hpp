#include "cli.hpp"

class CliServerCommand : public AbstractCommandLineCommand {
  std::string id() const override { return "server"; }
  std::string description() const override { return "(Re)start the vicinae server"; }

  void setup(CLI::App *app) override;
  void run(CLI::App *app) override;

private:
  bool m_open = false;
  bool m_noReplace = false;
};
