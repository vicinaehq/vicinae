#pragma once
#include "extensions/vicinae/manage-fallback/manage-fallback-view.hpp"
#include "ui/action-pannel/action.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto view = new ManageFallbackView();

    ctx->navigation->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction("Manage Fallback Actions", ImageURL::builtin("arrow-counter-clockwise")) {}
};
