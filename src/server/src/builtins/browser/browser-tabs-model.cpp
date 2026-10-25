#include "builtins/browser/browser-tabs-model.hpp"
#include "actions/browser-tab-actions.hpp"

QString BrowserTabsSection::displayTitle(const BrowserTab &tab) const { return tab.host(); }

QString BrowserTabsSection::displaySubtitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.title);
}

std::optional<ImageURL> BrowserTabsSection::displayIcon(const BrowserTab &tab) const { return tab.icon(); }

AccessoryList BrowserTabsSection::displayAccessories(const BrowserTab &tab) const {
  AccessoryList accessories;
  accessories.reserve(2);
  if (!tab.browserLabel.empty())
    accessories.emplace_back(ListAccessory{.text = QString::fromStdString(tab.browserLabel)});
  if (tab.audible) accessories.emplace_back(ListAccessory{.text = tab.muted ? tr("Muted") : tr("Playing")});
  return accessories;
}

std::unique_ptr<ActionPanelState> BrowserTabsSection::buildActionPanel(const BrowserTab &tab) const {
  return BrowserTabActionGenerator::generate(scope().appContext(), tab);
}
