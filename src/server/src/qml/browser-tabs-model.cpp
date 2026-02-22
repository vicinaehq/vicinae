#include "browser-tabs-model.hpp"
#include "actions/browser-tab-actions.hpp"

QString BrowserTabsModel::displayTitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.title);
}

QString BrowserTabsModel::displaySubtitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.url);
}

QString BrowserTabsModel::displayIconSource(const BrowserTab &tab) const {
  return imageSourceFor(tab.icon());
}

QVariantList BrowserTabsModel::displayAccessory(const BrowserTab &tab) const {
  if (tab.audible) return qml::textAccessory(tab.muted ? QStringLiteral("Muted") : QStringLiteral("Playing"));
  return {};
}

std::unique_ptr<ActionPanelState> BrowserTabsModel::buildActionPanel(const BrowserTab &tab) const {
  return BrowserTabActionGenerator::generate(scope().appContext(), tab);
}

QString BrowserTabsModel::sectionLabel() const {
  return QStringLiteral("Tabs ({count})");
}
