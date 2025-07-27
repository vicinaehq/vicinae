#pragma once
#include "manage-fallback-commands.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(AppWindow &app) override {}

  void execute(ApplicationContext *ctx) override {
    auto view = new ManageFallbackCommands();

    ctx->navigation->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction("Manage Fallback Actions", BuiltinOmniIconUrl("arrow-counter-clockwise")) {}
};
