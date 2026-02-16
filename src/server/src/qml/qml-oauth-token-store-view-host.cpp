#include "qml-oauth-token-store-view-host.hpp"
#include "qml-oauth-token-store-model.hpp"
#include "service-registry.hpp"
#include "services/oauth/oauth-service.hpp"

QUrl QmlOAuthTokenStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap QmlOAuthTokenStoreViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void QmlOAuthTokenStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlOAuthTokenStoreModel(this);
  m_model->initialize(context());

  setSearchPlaceholderText("Search token sets...");

  auto oauth = ServiceRegistry::instance()->oauthService();
  connect(&oauth->store(), &OAuth::TokenStore::setRemoved, this,
          &QmlOAuthTokenStoreViewHost::reload);
}

void QmlOAuthTokenStoreViewHost::loadInitialData() { reload(); }

void QmlOAuthTokenStoreViewHost::textChanged(const QString &text) {
  m_model->setFilter(text);
}

void QmlOAuthTokenStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

void QmlOAuthTokenStoreViewHost::beforePop() { m_model->beforePop(); }

QObject *QmlOAuthTokenStoreViewHost::listModel() const { return m_model; }

void QmlOAuthTokenStoreViewHost::reload() {
  auto oauth = ServiceRegistry::instance()->oauthService();
  m_model->setItems(oauth->store().list());
}
