#include "oauth-token-store-view-host.hpp"
#include "oauth-token-store-model.hpp"
#include "service-registry.hpp"
#include "services/oauth/oauth-service.hpp"

QUrl OAuthTokenStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
}

QVariantMap OAuthTokenStoreViewHost::qmlProperties() const {
  return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
}

void OAuthTokenStoreViewHost::initialize() {
  BaseView::initialize();

  m_model = new OAuthTokenStoreModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search token sets...");

  auto oauth = ServiceRegistry::instance()->oauthService();
  connect(&oauth->store(), &OAuth::TokenStore::setRemoved, this, &OAuthTokenStoreViewHost::reload);
}

void OAuthTokenStoreViewHost::loadInitialData() { reload(); }

void OAuthTokenStoreViewHost::textChanged(const QString &text) { m_model->setFilter(text); }

void OAuthTokenStoreViewHost::onReactivated() { m_model->refreshActionPanel(); }

void OAuthTokenStoreViewHost::beforePop() { m_model->beforePop(); }

QObject *OAuthTokenStoreViewHost::listModel() const { return m_model; }

void OAuthTokenStoreViewHost::reload() {
  auto oauth = ServiceRegistry::instance()->oauthService();
  m_model->setItems(oauth->store().list());
}
