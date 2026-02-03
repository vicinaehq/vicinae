#include "oauth-token-store-view.hpp"
#include "clipboard-actions.hpp"
#include "common.hpp"
#include "keyboard/keybind.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/views/list-view.hpp"
#include <memory>
#include <qdatetime.h>
#include "services/oauth/oauth-service.hpp"
#include "services/toast/toast-service.hpp"

class OAuthTokenSetItem : public SearchableListView::Actionnable {
public:
  OAuthTokenSetItem(const OAuth::TokenSet &set) : m_set(set) {}

  QString generateId() const override { return m_set.extensionId + m_set.providerId.value_or(""); }

  std::vector<QString> searchStrings() const override {
    return {m_set.providerId.value_or(""), m_set.extensionId};
  }

  ItemData data() const override {
    ItemData data;
    data.name = m_set.extensionId;
    data.subtitle = m_set.providerId.value_or("");

    data.iconUrl = ImageURL::builtin("key");

    if (m_set.isExpired()) {
      ListAccessory tag;
      tag.text = "Expired";
      tag.color = SemanticColor::Red;
      tag.icon = ImageURL::builtin("clock");
      data.accessories.emplace_back(tag);
    }

    if (auto scopes = m_set.scopes(); !scopes.empty()) {
      ListAccessory tag;
      tag.text = QString("%1 scope%2").arg(scopes.size()).arg(scopes.size() > 1 ? "s" : "");
      tag.color = SemanticColor::TextPrimary;
      tag.icon = ImageURL::builtin("coin");
      data.accessories.emplace_back(tag);
    }

    if (auto refresh = m_set.refreshToken) {
      ListAccessory tag;
      tag.icon = ImageURL::builtin("arrow-counter-clockwise");
      data.accessories.emplace_back(tag);
    }

    return data;
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    panel->setTitle(m_set.extensionId);

    // Primary section with uninstall
    auto primary = panel->createSection();

    auto removeToken =
        new StaticAction("Remove token set", ImageURL::builtin("trash"),
                         [id = m_set.extensionId, pid = m_set.providerId](ApplicationContext *ctx) {
                           auto oauth = ctx->services->oauthService();
                           auto toast = ctx->services->toastService();

                           ctx->navigation->confirmAlert("Are you sure?",
                                                         "You will need to go through the OAuth login flow "
                                                         "again the next time you want to use this service",
                                                         [oauth, toast, id, pid]() {
                                                           if (!oauth->store().removeTokenSet(id, pid)) {
                                                             toast->failure("Failed to remove token set");
                                                             return;
                                                           }

                                                           toast->success("Token set removed");
                                                         });
                         });

    removeToken->setStyle(AbstractAction::Style::Danger);
    primary->addAction(removeToken);

    // Copy actions section
    auto utils = panel->createSection("Copy");

    auto copyAccessToken = new CopyToClipboardAction(Clipboard::Text(m_set.accessToken), "Copy Access Token");
    copyAccessToken->setShortcut(Keybind::CopyAction);
    utils->addAction(copyAccessToken);

    if (m_set.refreshToken) {
      auto copyRefreshToken =
          new CopyToClipboardAction(Clipboard::Text(*m_set.refreshToken), "Copy Refresh Token");
      utils->addAction(copyRefreshToken);
    }

    if (m_set.idToken) {
      auto copyIdToken = new CopyToClipboardAction(Clipboard::Text(*m_set.idToken), "Copy ID Token");
      utils->addAction(copyIdToken);
    }

    if (m_set.scope) {
      auto copyScopes = new CopyToClipboardAction(Clipboard::Text(*m_set.scope), "Copy Scopes");
      utils->addAction(copyScopes);
    }

    if (auto date = m_set.expirationDate()) {
      auto copyExpirationDate =
          new CopyToClipboardAction(Clipboard::Text(date->toString()), "Copy Expiration Date");
      utils->addAction(copyExpirationDate);
    }

    return panel;
  }

private:
  OAuth::TokenSet m_set;
};

OAuthTokenStoreView::OAuthTokenStoreView() {
  auto oauth = ServiceRegistry::instance()->oauthService();
  connect(&oauth->store(), &OAuth::TokenStore::setRemoved, this, [this]() {
    setData(initData());
    textChanged(searchText());
  });
}

QString OAuthTokenStoreView::initialSearchPlaceholderText() const { return "Search token sets..."; }

SearchableListView::Data OAuthTokenStoreView::initData() const {
  auto oauth = ServiceRegistry::instance()->oauthService();
  auto sets = oauth->store().list();

  Data items;
  items.reserve(sets.size());

  for (const auto &set : sets) {
    items.push_back(std::make_shared<OAuthTokenSetItem>(set));
  }

  return items;
}
