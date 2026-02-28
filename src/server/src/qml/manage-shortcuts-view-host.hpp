#pragma once
#include "bridge-view.hpp"
#include <QVariantList>
#include <memory>

class ManageShortcutsModel;
class Shortcut;
class ShortcutService;

class ManageShortcutsViewHost : public ViewHostBase {
  Q_OBJECT

  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
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
  void textChanged(const QString &text) override;
  void onReactivated() override;
  void beforePop() override;

  QObject *listModel() const;
  bool hasDetail() const { return m_hasDetail; }
  QString detailContent() const { return m_detailContent; }
  QVariantList detailMetadata() const { return m_detailMetadata; }

private:
  void loadDetail(const std::shared_ptr<Shortcut> &shortcut);
  void updateExpandedUrl();
  void clearDetail();
  void reload();

  ManageShortcutsModel *m_model = nullptr;
  ShortcutService *m_shortcutService = nullptr;

  std::shared_ptr<Shortcut> m_currentShortcut;
  bool m_hasDetail = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
};
