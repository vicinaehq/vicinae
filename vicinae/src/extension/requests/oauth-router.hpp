#pragma once
#include "common.hpp"
#include "proto/extension.pb.h"
#include "proto/oauth.pb.h"
#include "services/oauth/oauth-service.hpp"
#include "types.hpp"
#include <qobject.h>

class OAuthRouter : public QObject {
public:
  PromiseLike<proto::ext::extension::Response *> route(const proto::ext::oauth::Request &req);
  OAuthRouter(const QString &extensionId, const ApplicationContext &ctx);

private:
  static proto::ext::extension::Response *wrapResponse(proto::ext::oauth::Response *oauthRes);

  QString computeTokenSetId(const QString &providerId) const;
  proto::ext::oauth::Response *getTokens(const proto::ext::oauth::GetTokensRequest &req);
  proto::ext::oauth::Response *setTokens(const proto::ext::oauth::SetTokensRequest &req);
  proto::ext::oauth::Response *removeTokens(const proto::ext::oauth::RemoveTokensRequest &req);

  QFuture<proto::ext::extension::Response *> authorize(const proto::ext::oauth::AuthorizeRequest &req);

  const ApplicationContext &m_ctx;
  OAuthService &m_oauth;
  QString m_extensionId;
};
