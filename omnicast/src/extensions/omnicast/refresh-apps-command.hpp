#pragma once
#include "command.hpp"

class RefreshAppsCommandContext : public CommandContext {
public:
  void load(const LaunchProps &props) override;
  RefreshAppsCommandContext(const std::shared_ptr<AbstractCmd> &command);
};
