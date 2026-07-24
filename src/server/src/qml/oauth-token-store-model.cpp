#include "oauth-token-store-model.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "services/oauth/oauth-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/image/url.hpp"

QString OAuthTokenStoreSection::displayTitle(const OAuth::TokenSet &set) const { return set.extensionId; }

QString OAuthTokenStoreSection::displaySubtitle(const OAuth::TokenSet &set) const {
  return set.providerId.value_or("");
}

QString OAuthTokenStoreSection::displayIconSource(const OAuth::TokenSet &set) const {
  return imageSourceFor(ImageURL::builtin("key"));
}

QVariantList OAuthTokenStoreSection::displayAccessories(const OAuth::TokenSet &set) const {
  if (set.isExpired()) return qml::textAccessory(tr("Expired"));
  return {};
}

std::unique_ptr<ActionPanelState> OAuthTokenStoreSection::buildActionPanel(const OAuth::TokenSet &set) const {
  auto panel = std::make_unique<ActionPanelState>();
  panel->setTitle(set.extensionId);

  auto primary = panel->createSection();

  auto removeToken =
      new StaticAction(tr("Remove token set"), ImageURL::builtin("trash"),
                       [id = set.extensionId, pid = set.providerId](ApplicationContext *ctx) {
                         auto oauth = ctx->services->oauthService();
                         auto toast = ctx->services->toastService();

                         ctx->navigation->confirmAlert(tr("Are you sure?"),
                                                       tr("You will need to go through the OAuth login flow "
                                                          "again the next time you want to use this service"),
                                                       [oauth, toast, id, pid]() {
                                                         if (!oauth->store().removeTokenSet(id, pid)) {
                                                           toast->failure(tr("Failed to remove token set"));
                                                           return;
                                                         }
                                                         toast->success(tr("Token set removed"));
                                                       });
                       });

  removeToken->setStyle(AbstractAction::Style::Danger);
  primary->addAction(removeToken);

  auto utils = panel->createSection(tr("Copy"));

  auto copyAccessToken = new CopyToClipboardAction(Clipboard::Text(set.accessToken), tr("Copy Access Token"));
  copyAccessToken->setShortcut(Keybind::CopyAction);
  utils->addAction(copyAccessToken);

  if (set.refreshToken) {
    auto copyRefreshToken =
        new CopyToClipboardAction(Clipboard::Text(*set.refreshToken), tr("Copy Refresh Token"));
    utils->addAction(copyRefreshToken);
  }

  if (set.idToken) {
    auto copyIdToken = new CopyToClipboardAction(Clipboard::Text(*set.idToken), tr("Copy ID Token"));
    utils->addAction(copyIdToken);
  }

  if (set.scope) {
    auto copyScopes = new CopyToClipboardAction(Clipboard::Text(*set.scope), tr("Copy Scopes"));
    utils->addAction(copyScopes);
  }

  if (auto date = set.expirationDate()) {
    auto copyExpirationDate =
        new CopyToClipboardAction(Clipboard::Text(date->toString()), tr("Copy Expiration Date"));
    utils->addAction(copyExpirationDate);
  }

  return panel;
}
