#include "browser-tabs-model.hpp"
#include "actions/browser-tab-actions.hpp"

QString BrowserTabsSection::displayTitle(const BrowserTab &tab) const { return tab.host(); }

QString BrowserTabsSection::displaySubtitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.title);
}

QString BrowserTabsSection::displayIconSource(const BrowserTab &tab) const {
  return imageSourceFor(tab.icon());
}

QVariantList BrowserTabsSection::displayAccessories(const BrowserTab &tab) const {
  if (tab.audible) return qml::textAccessory(tab.muted ? QStringLiteral("Muted") : QStringLiteral("Playing"));
  return {};
}

std::unique_ptr<ActionPanelState> BrowserTabsSection::buildActionPanel(const BrowserTab &tab) const {
  return BrowserTabActionGenerator::generate(scope().appContext(), tab);
}
