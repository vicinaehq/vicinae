#pragma once
#include "command.hpp"
#include "common.hpp"
#include "settings-controller/settings-controller.hpp"

class OpenAboutCommand : public CommandContext {
  void load(const LaunchProps &props) override { context()->settings->openTab("About"); }

public:
  OpenAboutCommand(const std::shared_ptr<AbstractCmd> &cmd) : CommandContext(cmd) {}
};
