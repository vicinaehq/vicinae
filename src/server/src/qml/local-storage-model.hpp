#pragma once
#include "fuzzy-section.hpp"
#include <QCoreApplication>

class LocalStorageNamespaceSection : public FuzzySection<QString> {
  Q_DECLARE_TR_FUNCTIONS(LocalStorageNamespaceSection)
public:
  QString sectionName() const override { return tr("Namespaces ({count})"); }

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
};

class LocalStorageItemSection : public FuzzySection<QString> {
  Q_DECLARE_TR_FUNCTIONS(LocalStorageItemSection)
public:
  QString sectionName() const override { return tr("Items ({count})"); }

  void setNamespace(const QString &ns) { m_ns = ns; }

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;

private:
  QString m_ns;
};
