#pragma once
#include "app.hpp"
#include "command-database.hpp"
#include "edit-command-preferences-view.hpp"
#include "omni-command-db.hpp"
#include "root-item-manager.hpp"
#include <libqalculate/includes.h>
#include <memory>
#include <ranges>

struct OpenBuiltinCommandAction : public AbstractAction {
  std::shared_ptr<AbstractCmd> cmd;
  QString text;

  void execute(AppWindow &app) override {
    LaunchProps props;
    props.arguments = app.topBar->m_completer->collect();

    app.launchCommand(cmd, {}, props);
  }

  OpenBuiltinCommandAction(const std::shared_ptr<AbstractCmd> &cmd, const QString &title = "Open command",
                           const QString &text = "")
      : AbstractAction(title, cmd->iconUrl()), cmd(cmd), text(text) {}
};

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

class CommandRootProvider : public RootProvider {
  OmniCommandDatabase &m_db;

  class CommandRootItem : public RootItem {
    std::shared_ptr<AbstractCmd> m_command;

    QString displayName() const override { return m_command->name(); }
    QString subtitle() const override { return m_command->repositoryName(); }
    OmniIconUrl iconUrl() const override { return m_command->iconUrl(); }
    ArgumentList arguments() const override { return m_command->arguments(); }

    QList<AbstractAction *> actions() const override {
      QList<AbstractAction *> actions{
          new OpenBuiltinCommandAction(m_command, "Open command"),
      };

      if (!m_command->preferences().empty()) { actions << new OpenCommandPreferencesAction(m_command); }

      return actions;
    }

    QString uniqueId() const override { return m_command->id(); }

    AccessoryList accessories() const override {
      return {{.text = "Command", .color = ColorTint::TextSecondary}};
    }

  public:
    CommandRootItem(const std::shared_ptr<AbstractCmd> &command) : m_command(command) {}
  };

public:
  QString displayName() const override { return "Commands"; }

  QString uniqueId() const override { return "commands"; }

  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_db.commands() | std::views::transform([](const auto &entry) {
             return std::static_pointer_cast<RootItem>(std::make_shared<CommandRootItem>(entry.command));
           }) |
           std::ranges::to<std::vector>();
  }

  CommandRootProvider(OmniCommandDatabase &m_commandDb) : m_db(m_commandDb) {
    connect(&m_commandDb, &OmniCommandDatabase::commandRegistered, this, [this]() { emit itemsChanged(); });
  }
};
