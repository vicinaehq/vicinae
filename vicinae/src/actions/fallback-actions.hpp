#pragma once
#include "manage-fallback-commands.hpp"
#include "../ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(AppWindow &app) override {}

  void execute(ApplicationContext *ctx) override {
    auto view = new ManageFallbackCommandsView();

    ctx->navigation->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction("Manage Fallback Actions", ImageURL::builtin("arrow-counter-clockwise")) {}
};
