#include "qml-browser-tabs-model.hpp"
#include "actions/browser-tab-actions.hpp"

QString QmlBrowserTabsModel::displayTitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.title);
}

QString QmlBrowserTabsModel::displaySubtitle(const BrowserTab &tab) const {
  return QString::fromStdString(tab.url);
}

QString QmlBrowserTabsModel::displayIconSource(const BrowserTab &tab) const {
  return imageSourceFor(tab.icon());
}

QString QmlBrowserTabsModel::displayAccessory(const BrowserTab &tab) const {
  if (tab.audible) return tab.muted ? QStringLiteral("Muted") : QStringLiteral("Playing");
  return {};
}

std::unique_ptr<ActionPanelState> QmlBrowserTabsModel::buildActionPanel(const BrowserTab &tab) const {
  return BrowserTabActionGenerator::generate(ctx(), tab);
}

QString QmlBrowserTabsModel::sectionLabel() const {
  return QStringLiteral("Tabs ({count})");
}
