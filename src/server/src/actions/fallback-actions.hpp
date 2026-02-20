#pragma once
#include "qml/qml-manage-fallback-view-host.hpp"
#include "ui/action-pannel/action.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto view = new QmlManageFallbackViewHost();

    ctx->navigation->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction("Manage Fallback Actions", ImageURL::builtin("arrow-counter-clockwise")) {}
};
