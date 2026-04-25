#pragma once
#include "fuzzy-list-model.hpp"
#include "services/browser-extension-service.hpp"

using BrowserTab = BrowserExtensionService::BrowserTab;

template <> struct fuzzy::FuzzySearchable<BrowserTab> {
  static int score(const BrowserTab &tab, std::string_view query) {
    // URL is intentionally weighted above title here: when switching tabs, users
    // overwhelmingly search by domain/path, not by the (often verbose) page title.
    return fuzzy::scoreWeighted({{tab.url, 1.0}, {tab.title, 0.6}}, query);
  }
};

class BrowserTabsModel : public FuzzyListModel<BrowserTab> {
  Q_OBJECT

public:
  using FuzzyListModel::FuzzyListModel;

protected:
  QString displayTitle(const BrowserTab &tab) const override;
  QString displaySubtitle(const BrowserTab &tab) const override;
  QString displayIconSource(const BrowserTab &tab) const override;
  QVariantList displayAccessory(const BrowserTab &tab) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserTab &tab) const override;
  QString sectionLabel() const override;
};
