#pragma once
#include "ui/views/detail-list-view-host.hpp"
#include "builtins/shortcut/manage-shortcuts-model.hpp"
#include <QVariantList>
#include <memory>

class ShortcutService;

class ManageShortcutsViewHost : public DetailListViewHost {
  Q_OBJECT

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  bool hasDetail() const override { return m_hasDetail; }
  QString detailContent() const override { return m_detailContent; }
  QVariantList detailMetadata() const override { return m_detailMetadata; }

private:
  void loadDetail(const std::shared_ptr<Shortcut> &shortcut);
  void updateExpandedUrl();
  void clearDetail();
  void reload();

  ManageShortcutsSection m_section;
  ShortcutService *m_shortcutService = nullptr;

  std::shared_ptr<Shortcut> m_currentShortcut;
  bool m_hasDetail = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
};
