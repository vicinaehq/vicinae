#pragma once
#include "qml-font-browser-model.hpp"

class QmlLocalStorageNamespaceModel : public QmlFuzzyListModel<QString> {
  Q_OBJECT
public:
  using QmlFuzzyListModel::QmlFuzzyListModel;

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
  QString sectionLabel() const override { return QStringLiteral("Namespaces ({count})"); }
};

class QmlLocalStorageItemModel : public QmlFuzzyListModel<QString> {
  Q_OBJECT
public:
  using QmlFuzzyListModel::QmlFuzzyListModel;

  void setNamespace(const QString &ns) { m_ns = ns; }

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
  QString sectionLabel() const override { return QStringLiteral("Items ({count})"); }

private:
  QString m_ns;
};
