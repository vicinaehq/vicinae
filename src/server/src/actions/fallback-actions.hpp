#pragma once
#include <QCoreApplication>
#include "extensions/vicinae/manage-fallback-view-host.hpp"
#include "ui/action-panel/action.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto view = new ManageFallbackViewHost();

    ctx->navigation->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction(QCoreApplication::translate("ManageFallbackActions", "Manage Fallback Actions"),
                       ImageURL::builtin(BuiltinIcon::ArrowCounterClockwise)) {}
};
