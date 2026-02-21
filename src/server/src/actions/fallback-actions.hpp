#pragma once
#include "qml/manage-fallback-view-host.hpp"
#include "ui/action-pannel/action.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto view = new ManageFallbackViewHost();

    ctx->navigation->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction("Manage Fallback Actions", ImageURL::builtin("arrow-counter-clockwise")) {}
};
