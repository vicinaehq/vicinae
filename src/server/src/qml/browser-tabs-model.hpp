#pragma once
#include "fuzzy-section.hpp"
#include "services/browser-extension-service.hpp"
#include <QCoreApplication>

using BrowserTab = BrowserExtensionService::BrowserTab;

template <> struct fuzzy::FuzzySearchable<BrowserTab> {
  static int score(const BrowserTab &tab, std::string_view query) {
    return fuzzy::scoreWeighted({{tab.url, 1.0}, {tab.title, 0.6}}, query);
  }
};

class BrowserTabsSection : public FuzzySection<BrowserTab> {
  Q_DECLARE_TR_FUNCTIONS(BrowserTabsSection)
public:
  QString sectionName() const override { return tr("Tabs ({count})"); }

protected:
  QString displayTitle(const BrowserTab &tab) const override;
  QString displaySubtitle(const BrowserTab &tab) const override;
  QString displayIconSource(const BrowserTab &tab) const override;
  QVariantList displayAccessories(const BrowserTab &tab) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserTab &tab) const override;
};
