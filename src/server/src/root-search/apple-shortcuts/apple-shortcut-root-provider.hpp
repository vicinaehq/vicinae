#pragma once

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QTimer>

#include "services/apple-shortcuts/apple-shortcuts.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class ToastService;

class AppleShortcutRootItem : public RootItem {
  Q_DECLARE_TR_FUNCTIONS(AppleShortcutRootItem)

public:
  explicit AppleShortcutRootItem(AppleShortcuts::Shortcut shortcut);

  EntrypointId uniqueId() const override;
  QString title() const override;
  QString typeDisplayName() const override;
  ImageURL iconUrl() const override;
  AccessoryList accessories() const override;
  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override;

private:
  AppleShortcuts::Shortcut m_shortcut;
  ImageURL m_icon;
};

class AppleShortcutRootProvider : public RootProvider {
  Q_OBJECT

public:
  explicit AppleShortcutRootProvider(ToastService &toastService);

  QString uniqueId() const override;
  QString displayName() const override;
  ImageURL icon() const override;
  Type type() const override;
  void initialized(const PreferenceValues &preferences) override;
  std::vector<std::shared_ptr<RootItem>> loadItems() const override;

private:
  void refresh();

  ToastService &m_toastService;
  QTimer m_refreshTimer;
  QElapsedTimer m_lastRefresh;
  QFutureWatcher<AppleShortcuts::ListResult> m_scanWatcher;
  std::vector<AppleShortcuts::Shortcut> m_shortcuts;
  std::optional<QString> m_lastError;
};
