#pragma once
#include "fuzzy-section.hpp"

class LocalStorageNamespaceSection : public FuzzySection<QString> {
public:
  QString sectionName() const override { return QStringLiteral("Namespaces ({count})"); }

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;
};

class LocalStorageItemSection : public FuzzySection<QString> {
public:
  QString sectionName() const override { return QStringLiteral("Items ({count})"); }

  void setNamespace(const QString &ns) { m_ns = ns; }

protected:
  QString displayTitle(const QString &item) const override { return item; }
  QString displayIconSource(const QString &item) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const QString &item) const override;

private:
  QString m_ns;
};
