#pragma once
#include "common.hpp"
#include "proto/extension.pb.h"
#include "proto/oauth.pb.h"
#include "types.hpp"
#include <qobject.h>

class OAuthRouter : public QObject {
public:
  PromiseLike<proto::ext::extension::Response *> route(const proto::ext::oauth::Request &req);
  OAuthRouter(const ApplicationContext &oauth);

private:
  static proto::ext::extension::Response *wrapResponse(proto::ext::oauth::Response *wmRes);
  QFuture<proto::ext::extension::Response *> authorize(const proto::ext::oauth::AuthorizeRequest &req);

  const ApplicationContext &m_ctx;
};
