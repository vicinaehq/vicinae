#include "oauth-token-store-view-host.hpp"
#include "oauth-token-store-model.hpp"
#include "service-registry.hpp"
#include "services/oauth/oauth-service.hpp"

void OAuthTokenStoreViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText("Search token sets...");

  auto oauth = ServiceRegistry::instance()->oauthService();
  connect(&oauth->store(), &OAuth::TokenStore::setRemoved, this, &OAuthTokenStoreViewHost::reload);
}

void OAuthTokenStoreViewHost::loadInitialData() { reload(); }

void OAuthTokenStoreViewHost::reload() {
  auto oauth = ServiceRegistry::instance()->oauthService();
  m_section.setItems(oauth->store().list());
}
