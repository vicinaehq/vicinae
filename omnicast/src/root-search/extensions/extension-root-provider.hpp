#pragma once
#include "root-item-manager.hpp"

class CommandRootItem : public RootItem {
  std::shared_ptr<AbstractCmd> m_command;

public:
  QString displayName() const override;
  QString subtitle() const override;
  OmniIconUrl iconUrl() const override;
  ArgumentList arguments() const override;
  QString providerId() const override;
  bool isSuitableForFallback() const override;
  double baseScoreWeight() const override;
  ActionPanelView *actionPanel() const override;
  ActionPanelView *fallbackActionPanel() const override;
  QString uniqueId() const override;
  AccessoryList accessories() const override;
  PreferenceList preferences() const override { return m_command->preferences(); }

public:
  auto command() const { return m_command; }
  CommandRootItem(const std::shared_ptr<AbstractCmd> &command) : m_command(command) {}
};

class ExtensionRootProvider : public RootProvider {
  std::shared_ptr<AbstractCommandRepository> m_repo;

public:
  PreferenceList preferences() const override { return m_repo->preferences(); }
  QString displayName() const override { return m_repo->name(); }
  QString uniqueId() const override { return QString("extension.%1").arg(m_repo->id()); }
  Type type() const override { return RootProvider::Type::ExtensionProvider; }
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

  ExtensionRootProvider(const std::shared_ptr<AbstractCommandRepository> &repo) : m_repo(repo) {}
};
