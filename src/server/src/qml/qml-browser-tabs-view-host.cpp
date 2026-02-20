#include "qml-browser-tabs-view-host.hpp"
#include "qml-browser-tabs-model.hpp"
#include "service-registry.hpp"

QUrl QmlBrowserTabsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlBrowserTabsViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlBrowserTabsViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlBrowserTabsModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search, focus and close tabs");

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::tabsChanged, this, &QmlBrowserTabsViewHost::reload);
}

void QmlBrowserTabsViewHost::loadInitialData() { reload(); }

void QmlBrowserTabsViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void QmlBrowserTabsViewHost::onReactivated() { m_model->refreshActionPanel(); }

void QmlBrowserTabsViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlBrowserTabsViewHost::listModel() const { return m_model; }

void QmlBrowserTabsViewHost::reload() {
  m_model->setItems(context()->services->browserExtension()->tabs());
}
