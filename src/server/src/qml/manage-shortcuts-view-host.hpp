#pragma once
#include "list-view-host.hpp"
#include "manage-shortcuts-model.hpp"
#include <QVariantList>
#include <memory>

class ShortcutService;

class ManageShortcutsViewHost : public ListViewHost {
  Q_OBJECT

  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailContent READ detailContent NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void beforePop() override;

  bool hasDetail() const { return m_hasDetail; }
  QString detailContent() const { return m_detailContent; }
  QVariantList detailMetadata() const { return m_detailMetadata; }

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
