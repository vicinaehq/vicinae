#pragma once
#include "command-actions.hpp"

class CommandRootItem : public RootItem {
  std::shared_ptr<AbstractCmd> m_command;

  QString displayName() const override { return m_command->name(); }
  QString subtitle() const override { return m_command->repositoryName(); }
  OmniIconUrl iconUrl() const override { return m_command->iconUrl(); }
  ArgumentList arguments() const override { return m_command->arguments(); }
  QString providerId() const override { return "command"; }

  double baseScoreWeight() const override { return 1.1; }

  QList<AbstractAction *> actions() const override {
    QList<AbstractAction *> actions{
        new OpenBuiltinCommandAction(m_command, "Open command"),
    };

    // if (!m_command->preferences().empty()) { actions << new OpenCommandPreferencesAction(m_command); }

    return actions;
  }

  QString uniqueId() const override { return QString("extension.%1").arg(m_command->uniqueId()); }

  AccessoryList accessories() const override {
    return {{.text = "Command", .color = ColorTint::TextSecondary}};
  }

public:
  CommandRootItem(const std::shared_ptr<AbstractCmd> &command) : m_command(command) {}
};

class ExtensionRootProvider : public RootProvider {
  std::shared_ptr<AbstractCommandRepository> m_repo;

public:
  QString displayName() const override { return m_repo->name(); }

  QString uniqueId() const override { return QString("extension.%1").arg(m_repo->id()); }

  Type type() const override { return RootProvider::Type::ExtensionProvider; }

  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_repo->commands() | std::views::transform([](const auto &command) {
             return std::static_pointer_cast<RootItem>(std::make_shared<CommandRootItem>(command));
           }) |
           std::ranges::to<std::vector>();
  }

  ExtensionRootProvider(const std::shared_ptr<AbstractCommandRepository> &repo) : m_repo(repo) {}
};
