#include "oauth-router.hpp"
#include "common.hpp"
#include "proto/oauth.pb.h"
#include "services/oauth/oauth-service.hpp"
#include "service-registry.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "ui/oauth-view.hpp"

namespace oauth = proto::ext::oauth;

OAuthRouter::OAuthRouter(const ApplicationContext &ctx) : m_ctx(ctx) {}

PromiseLike<proto::ext::extension::Response *> OAuthRouter::route(const proto::ext::oauth::Request &req) {
  switch (req.payload_case()) {
  case oauth::Request::kAuthorize:
    return authorize(req.authorize());
  default:
    break;
  }

  return nullptr;
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
