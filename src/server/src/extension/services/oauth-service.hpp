#pragma once
#include "common/context.hpp"
#include "service-registry.hpp"
#include "generated/tsapi.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "qml/oauth-overlay-host.hpp"
#include "services/oauth/oauth-service.hpp"
#include <QFutureWatcher>

class ExtOAuthService : public tsapi::AbstractOAuth {
  using Void = tsapi::Result<void>;

public:
  ExtOAuthService(tsapi::RpcTransport &transport, const QString &extensionId, const ApplicationContext &ctx)
      : AbstractOAuth(transport), m_extensionId(extensionId), m_ctx(ctx),
        m_oauth(*ctx.services->oauthService()) {}

  tsapi::Result<tsapi::AuthorizeResponse>::Future authorize(tsapi::AuthorizeRequest payload) override {
    QUrl const url(QString::fromStdString(payload.url));
    QUrlQuery const query(url.query());
    QString const state = query.queryItemValue("state");

    auto host = new OAuthOverlayHost(&m_ctx, payload);
    m_ctx.overlay->setCurrent(host);

    auto *watcher = new QFutureWatcher<OAuthResponse>(this);
    QPromise<tsapi::Result<tsapi::AuthorizeResponse>::Type> promise;
    auto future = promise.future();

    connect(watcher, &QFutureWatcherBase::finished, this,
            [host, watcher, promise = std::move(promise)]() mutable {
              auto oauthRes = watcher->result();
              if (!oauthRes) {
                promise.addResult(std::unexpected(oauthRes.error().toStdString()));
              } else {
                host->showSuccess();
                promise.addResult(tsapi::AuthorizeResponse{.code = oauthRes->code.toStdString()});
              }
              promise.finish();
              watcher->deleteLater();
            });

    watcher->setFuture(m_oauth.authorize(state));

    return future;
  }

  tsapi::Result<tsapi::TokenSetResponse>::Future getTokens(std::optional<std::string> id) override {
    std::optional<QString> providerId;
    if (id) providerId = QString::fromStdString(*id);

    auto set = m_oauth.store().getTokenSet(m_extensionId, providerId);

    if (!set) return tsapi::Result<tsapi::TokenSetResponse>::ok({});

    tsapi::TokenSet result;
    result.accessToken = set->accessToken.toStdString();
    result.updatedAt = set->updatedAt;
    if (set->refreshToken) result.refreshToken = set->refreshToken->toStdString();
    if (set->idToken) result.idToken = set->idToken->toStdString();
    if (set->scope) result.scope = set->scope->toStdString();
    if (set->expiresIn) result.expiresIn = *set->expiresIn;

    return tsapi::Result<tsapi::TokenSetResponse>::ok({result});
  }

  Void::Future setTokens(tsapi::SetTokensRequest payload) override {
    OAuth::SetTokenSetPayload p;
    p.extensionId = m_extensionId;
    p.accessToken = QString::fromStdString(payload.accessToken);
    if (payload.providerId) p.providerId = QString::fromStdString(*payload.providerId);
    if (payload.refreshToken) p.refreshToken = QString::fromStdString(*payload.refreshToken);
    if (payload.idToken) p.idToken = QString::fromStdString(*payload.idToken);
    if (payload.scope) p.scope = QString::fromStdString(*payload.scope);
    if (payload.expiresIn) p.expiresIn = *payload.expiresIn;

    if (!m_oauth.store().setTokenSet(p)) return Void::fail("Failed to store token set");

    return Void::ok();
  }

  Void::Future removeTokens(std::optional<std::string> id) override {
    std::optional<QString> providerId;
    if (id) providerId = QString::fromStdString(*id);

    if (!m_oauth.store().removeTokenSet(m_extensionId, providerId))
      return Void::fail("Failed to remove token set");

    return Void::ok();
  }

private:
  QString m_extensionId;
  const ApplicationContext &m_ctx;
  OAuthService &m_oauth;
};
