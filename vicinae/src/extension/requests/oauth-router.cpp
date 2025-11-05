#include "oauth-router.hpp"
#include "common.hpp"
#include "proto/extension.pb.h"
#include "proto/oauth.pb.h"
#include "services/oauth/oauth-service.hpp"
#include "service-registry.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "ui/oauth-view.hpp"
#include <stdexcept>

namespace oauth = proto::ext::oauth;

OAuthRouter::OAuthRouter(const ApplicationContext &ctx) : m_ctx(ctx) {}

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
  throw std::runtime_error("getTokens: not implemented");
}

oauth::Response *OAuthRouter::setTokens(const oauth::SetTokensRequest &req) {
  throw std::runtime_error("setTokens: not implemented");
}

oauth::Response *OAuthRouter::removeTokens(const oauth::RemoveTokensRequest &req) {
  throw std::runtime_error("removeTokens: not implemented");
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

    qDebug() << "Got oauth response" << oauthRes->code;

    authorizeRes->set_code(oauthRes->code.toStdString());
    ores->set_allocated_authorize(authorizeRes);
    resData->set_allocated_oauth(ores);
    res->set_allocated_data(resData);
    return res;
  });
}
