#pragma once
#include "cli/cli.hpp"

class ThemeCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "theme"; }
  std::string description() const override { return "Theme-related commands"; }
  void setup(CLI::App *app) override { app->alias("th"); }

  ThemeCommand();
};
