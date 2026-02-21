#include "browser-tabs-view-host.hpp"
#include "browser-tabs-model.hpp"
#include "service-registry.hpp"

QUrl BrowserTabsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap BrowserTabsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void BrowserTabsViewHost::initialize() {
  BaseView::initialize();

  m_model = new BrowserTabsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search, focus and close tabs");

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::tabsChanged, this, &BrowserTabsViewHost::reload);
}

void BrowserTabsViewHost::loadInitialData() { reload(); }

void BrowserTabsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void BrowserTabsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void BrowserTabsViewHost::beforePop() { m_model->beforePop(); }

QObject *BrowserTabsViewHost::listModel() const { return m_model; }

void BrowserTabsViewHost::reload() {
  m_model->setItems(context()->services->browserExtension()->tabs());
}
