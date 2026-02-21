#pragma once
#include "font-browser-model.hpp"

class LocalStorageNamespaceModel : public FuzzyListModel<QString> {
  Q_OBJECT
public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
  QString sectionLabel() const override { return QStringLiteral("Namespaces ({count})"); }
};

class LocalStorageItemModel : public FuzzyListModel<QString> {
  Q_OBJECT
public:
  using FuzzyListModel::FuzzyListModel;

  void setNamespace(const QString &ns) { m_ns = ns; }

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
  QString sectionLabel() const override { return QStringLiteral("Items ({count})"); }

private:
  QString m_ns;
};
