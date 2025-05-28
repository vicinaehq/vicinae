#pragma once
#include "action-panel/action-panel.hpp"
#include "app/app-database.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"

class OpenAppAction : public AbstractAction {
  std::shared_ptr<Application> application;
  std::vector<QString> args;

  void execute() override;

public:
  OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                const std::vector<QString> args);
};

// Provides a submenu to select an app
class OpenWithAppAction : public AbstractAction {
  std::vector<QString> m_arguments;

  bool isSubmenu() const override { return true; }

  ActionPanelView *createSubmenu() const override;

public:
  OpenWithAppAction(const std::vector<QString> &arguments)
      : AbstractAction("Open with...", BuiltinOmniIconUrl("arrow-clockwise")), m_arguments(arguments) {}
};
