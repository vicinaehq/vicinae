#pragma once
#include "command.hpp"
#include "common.hpp"

class OpenDocumentationCommand : public CommandContext {
  void load(const LaunchProps &props) override;

public:
  OpenDocumentationCommand(const std::shared_ptr<AbstractCmd> &cmd);
};
