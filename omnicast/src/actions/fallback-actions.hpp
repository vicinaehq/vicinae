#pragma once
#include "manage-fallback-commands.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/ui-controller.hpp"

class ManageFallbackActions : public AbstractAction {
  void execute(AppWindow &app) override {}

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto view = new ManageFallbackCommands();

    ui->pushView(view);
  }

public:
  ManageFallbackActions()
      : AbstractAction("Manage Fallback Actions", BuiltinOmniIconUrl("arrow-counter-clockwise")) {}
};
