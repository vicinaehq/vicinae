#include "browser-tabs-model.hpp"
#include "actions/browser-tab-actions.hpp"

QString BrowserTabsSection::displayTitle(const BrowserTab &tab) const { return tab.host(); }

QString BrowserTabsSection::displaySubtitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.title);
}

std::optional<ImageURL> BrowserTabsSection::displayIcon(const BrowserTab &tab) const { return tab.icon(); }

AccessoryList BrowserTabsSection::displayAccessories(const BrowserTab &tab) const {
  if (tab.audible) return {{.text = tab.muted ? tr("Muted") : tr("Playing")}};
  return {};
}

std::unique_ptr<ActionPanelState> BrowserTabsSection::buildActionPanel(const BrowserTab &tab) const {
  return BrowserTabActionGenerator::generate(scope().appContext(), tab);
}
