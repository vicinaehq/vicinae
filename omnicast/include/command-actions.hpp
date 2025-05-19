#pragma once
#include "app.hpp"
#include "service-registry.hpp"

struct OpenBuiltinCommandAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> cmd;
  QString text;

  void execute() override {}

  void execute(AppWindow &app) override {
    LaunchProps props;
    props.arguments = app.topBar->m_completer->collect();

    app.launchCommand(cmd, {}, props);
  }

  OpenBuiltinCommandAction(const std::shared_ptr<AbstractCmd> &cmd, const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, cmd->iconUrl()), cmd(cmd), text(text) {}
};

/*
class OpenCommandPreferencesAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> m_command;

  void execute(AppWindow &app) override {
    app.pushView(new EditCommandPreferencesView(app, m_command),
                 {.navigation = NavigationStatus{.title = QString("%1 - Preferences").arg(m_command->name()),
                                                 .iconUrl = m_command->iconUrl()}});
  }

public:
  OpenCommandPreferencesAction(const std::shared_ptr<AbstractCmd> &command)
      : AbstractAction("Edit preferences", BuiltinOmniIconUrl("pencil")), m_command(command) {}
};
*/
