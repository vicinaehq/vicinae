#include "oauth-router.hpp"
#include "common.hpp"
#include "proto/extension.pb.h"
#include "proto/oauth.pb.h"
#include "services/oauth/oauth-service.hpp"
#include "service-registry.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "ui/oauth-view/oauth-view.hpp"
#include <stdexcept>

namespace oauth = proto::ext::oauth;

OAuthRouter::OAuthRouter(const QString &id, const ApplicationContext &ctx)
    : m_ctx(ctx), m_oauth(*ctx.services->oauthService()), m_extensionId(id) {}

PromiseLike<proto::ext::extension::Response *> OAuthRouter::route(const proto::ext::oauth::Request &req) {
  using Req = oauth::Request;
  using Res = oauth::Response;

  auto wrapRes = [](Res *oauthRes) {
    auto res = new proto::ext::extension::Response;
    auto data = new proto::ext::extension::ResponseData;
    data->set_allocated_oauth(oauthRes);
    res->set_allocated_data(data);
    return res;
  };

  switch (req.payload_case()) {
  case Req::kAuthorize:
    return authorize(req.authorize());
  case Req::kGetTokens:
    return wrapRes(getTokens(req.get_tokens()));
  case Req::kSetTokens:
    return wrapRes(setTokens(req.set_tokens()));
  case Req::kRemoveTokens:
    return wrapRes(removeTokens(req.remove_tokens()));
  default:
    break;
  }

  return nullptr;
}

oauth::Response *OAuthRouter::getTokens(const oauth::GetTokensRequest &req) {
  auto res = new oauth::Response;
  auto get = new oauth::GetTokensResponse;
  std::optional<QString> providerId;

  res->set_allocated_get_tokens(get);

  if (req.has_provider_id()) { providerId = req.provider_id().c_str(); }

  auto set = m_oauth.store().getTokenSet(m_extensionId, providerId);

  if (!set) return res;

  auto sset = new oauth::TokenSet;
  get->set_allocated_token_set(sset);
  sset->set_access_token(set->accessToken.toStdString());
  sset->set_updated_at(set->updatedAt);
  if (auto v = set->refreshToken) { sset->set_refresh_token(v->toStdString()); }
  if (auto v = set->idToken) { sset->set_id_token(v->toStdString()); }
  if (auto v = set->scope) { sset->set_scope(v->toStdString()); }
  if (auto v = set->expiresIn) { sset->set_expires_in(*v); }

  return res;
}

oauth::Response *OAuthRouter::setTokens(const oauth::SetTokensRequest &req) {
  OAuth::SetTokenSetPayload payload;

  payload.extensionId = m_extensionId;

  if (req.has_provider_id()) payload.providerId = req.provider_id().c_str();

  payload.accessToken = req.access_token().c_str();

  if (req.has_refresh_token()) { payload.refreshToken = req.refresh_token().c_str(); }
  if (req.has_id_token()) { payload.idToken = req.id_token().c_str(); }
  if (req.has_scope()) { payload.scope = req.scope().c_str(); }
  if (req.has_expires_in()) { payload.expiresIn = req.expires_in(); }

  if (!m_oauth.store().setTokenSet(payload)) { throw std::runtime_error("Failed to store token set"); }

  return nullptr;
}

oauth::Response *OAuthRouter::removeTokens(const oauth::RemoveTokensRequest &req) {
  std::optional<QString> providerId;

  if (req.has_provider_id()) providerId = req.provider_id().c_str();

  if (!m_oauth.store().removeTokenSet(m_extensionId, providerId)) {
    throw std::runtime_error("Failed to remove token set");
  }

  return nullptr;
}

QString OAuthRouter::computeTokenSetId(const QString &providerId) const {
  return QString("%1.%2").arg(m_extensionId).arg(providerId);
}

QFuture<proto::ext::extension::Response *>
OAuthRouter::authorize(const proto::ext::oauth::AuthorizeRequest &req) {
  auto oauth = m_ctx.services->oauthService();
  QUrl url(req.url().c_str());
  QUrlQuery query(url.query());
  QString state = query.queryItemValue("state");

  if (state.isEmpty()) { throw std::runtime_error("OAuth authorize request url should have a state param"); }

  std::cout << url.toString().toStdString() << std::endl;

  auto view = new OAuthView(&m_ctx, req);

  m_ctx.overlay->setCurrent(view);

  return oauth->authorize(state).then([view](const OAuthResponse &oauthRes) {
    auto res = new proto::ext::extension::Response;
    auto resData = new proto::ext::extension::ResponseData;
    auto ores = new oauth::Response;
    auto authorizeRes = new oauth::AuthorizeResponse();

    view->showSuccess();
    authorizeRes->set_code(oauthRes->code.toStdString());
    ores->set_allocated_authorize(authorizeRes);
    resData->set_allocated_oauth(ores);
    res->set_allocated_data(resData);
    return res;
  });
}
