#pragma once
#include "root-item-manager.hpp"
#include <qjsonobject.h>
#include <qwidget.h>
#include "settings/command-metadata-settings-detail.hpp"
#include "settings/extension-settings-detail.hpp"

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
  QString typeDisplayName() const override;
  QString uniqueId() const override;
  AccessoryList accessories() const override;
  PreferenceList preferences() const override { return m_command->preferences(); }
  QWidget *settingsDetail(const QJsonObject &preferences) const override {
    return new CommandMetadataSettingsDetailWidget(uniqueId(), m_command);
  }

  void preferenceValuesChanged(const QJsonObject &values) const override {
    m_command->preferenceValuesChanged(values);
  }

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
  OmniIconUrl icon() const override { return m_repo->iconUrl(); };
  Type type() const override { return RootProvider::Type::ExtensionProvider; }
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;
  QWidget *settingsDetail() const override { return new ExtensionSettingsDetail(uniqueId(), m_repo); }

  ExtensionRootProvider(const std::shared_ptr<AbstractCommandRepository> &repo) : m_repo(repo) {}
};
