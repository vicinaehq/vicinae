#pragma once
#include "qml-fuzzy-list-model.hpp"
#include "services/browser-extension-service.hpp"

using BrowserTab = BrowserExtensionService::BrowserTab;

template <> struct fuzzy::FuzzySearchable<BrowserTab> {
  static int score(const BrowserTab &tab, std::string_view query) {
    return fuzzy::scoreWeighted({{tab.title, 1.0}, {tab.url, 0.7}}, query);
  }
};

class QmlBrowserTabsModel : public QmlFuzzyListModel<BrowserTab> {
  Q_OBJECT

public:
  using QmlFuzzyListModel::QmlFuzzyListModel;

protected:
  QString displayTitle(const BrowserTab &tab) const override;
  QString displaySubtitle(const BrowserTab &tab) const override;
  QString displayIconSource(const BrowserTab &tab) const override;
  QString displayAccessory(const BrowserTab &tab) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const BrowserTab &tab) const override;
  QString sectionLabel() const override;
};
