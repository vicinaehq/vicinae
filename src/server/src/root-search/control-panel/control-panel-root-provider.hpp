#pragma once
#include "services/root-item-manager/root-item-manager.hpp"
#include <QCoreApplication>

struct WinControlPanelApplet {
  QString displayName;
  QString parsingName;
};

// A task from the "All Tasks" shell folder (god mode). Tasks cannot be re-created from a parsing
// name (the folder does not implement ParseDisplayName), so the absolute PIDL is kept for launching.
struct WinControlPanelTask {
  QString displayName;
  QString taskId; // in-folder parsing name, "{guid}"
  QByteArray pidl;
};

class WinControlPanelRootItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(WinControlPanelRootItem)

  WinControlPanelApplet m_applet;

  QString title() const override;
  QString typeDisplayName() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  AccessoryList accessories() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::vector<std::pair<QString, QString>> settingsMetadata() const override;

public:
  explicit WinControlPanelRootItem(WinControlPanelApplet applet) : m_applet(std::move(applet)) {}
};

class WinControlPanelTaskRootItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(WinControlPanelTaskRootItem)

  WinControlPanelTask m_task;

  QString title() const override;
  QString typeDisplayName() const override;
  ImageURL iconUrl() const override;
  EntrypointId uniqueId() const override;
  AccessoryList accessories() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;
  std::vector<std::pair<QString, QString>> settingsMetadata() const override;

public:
  explicit WinControlPanelTaskRootItem(WinControlPanelTask task) : m_task(std::move(task)) {}
};

class WinControlPanelRootProvider : public RootProvider {
  Q_DECLARE_TR_FUNCTIONS(WinControlPanelRootProvider)

public:
  QString uniqueId() const override;
  QString displayName() const override;
  QString description() const override;
  ImageURL icon() const override;
  Type type() const override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;
};
