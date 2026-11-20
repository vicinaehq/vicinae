#pragma once
#include <QCoreApplication>
#include "services/ai-commands/ai-command-store.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class RootAiCommandItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(RootAiCommandItem)

public:
  explicit RootAiCommandItem(AiCommandStore::Command command);

  EntrypointId uniqueId() const override;
  QString title() const override;
  ImageURL iconUrl() const override;
  QString typeDisplayName() const override;
  AccessoryList accessories() const override;
  ArgumentList arguments() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;

private:
  AiCommandStore::Command m_command;
};

class AiCommandRootProvider : public RootProvider {
public:
  explicit AiCommandRootProvider(AiCommandStore &store);

  QString displayName() const override;
  QString uniqueId() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

private:
  AiCommandStore &m_store;
};
