#pragma once
#include "common.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <qjsonobject.h>
#include <qwidget.h>
#include "settings/extension-settings-detail.hpp"
#include "common/entrypoint.hpp"

class CommandRootItem : public RootItem {
  std::shared_ptr<AbstractCmd> m_command;

public:
  QString displayName() const override;
  QString subtitle() const override;
  ImageURL iconUrl() const override;
  ArgumentList arguments() const override;
  std::vector<QString> keywords() const override { return m_command->keywords(); }
  bool isSuitableForFallback() const override;
  double baseScoreWeight() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::unique_ptr<ActionPanelState> fallbackActionPanel(ApplicationContext *ctx,
                                                        const RootItemMetadata &metadata) const override;

  bool supportsAliasSpaceShortcut() const override { return m_command->isView(); }

  QString typeDisplayName() const override;
  EntrypointId uniqueId() const override;
  AccessoryList accessories() const override;
  PreferenceList preferences() const override { return m_command->preferences(); }
  bool isDefaultDisabled() const override { return m_command->isDefaultDisabled(); }
  QWidget *settingsDetail(const QJsonObject &preferences) const override {
    return new CommandMetadataSettingsDetailWidget(m_command);
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
  const std::shared_ptr<AbstractCommandRepository> &repository() const { return m_repo; }
  PreferenceList preferences() const override { return m_repo->preferences(); }
  QString displayName() const override { return m_repo->displayName(); }
  QString uniqueId() const override { return repositoryId(); }
  ImageURL icon() const override { return m_repo->iconUrl(); };
  Type type() const override { return RootProvider::Type::ExtensionProvider; }
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;
  QWidget *settingsDetail() const override { return new ExtensionSettingsDetail(uniqueId(), m_repo); }

  void preferencesChanged(const QJsonObject &preferences) override {
    return m_repo->preferenceValuesChanged(preferences);
  }

  void initialized(const QJsonObject &preferences) override { return m_repo->initialized(preferences); }

  QString repositoryId() const { return m_repo->id(); }
  bool isBuiltin() const { return m_repo->author() == "vicinae"; }

  ExtensionRootProvider(const std::shared_ptr<AbstractCommandRepository> &repo) : m_repo(repo) {}
};
