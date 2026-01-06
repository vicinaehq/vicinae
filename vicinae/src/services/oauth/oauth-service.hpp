#pragma once
#include "ui/image/url.hpp"
#include "omni-database.hpp"
#include "proto/oauth.pb.h"
#include <expected>
#include "oauth-token-store.hpp"
#include <qfuture.h>
#include <qlogging.h>
#include <qpromise.h>
#include <qstring.h>
#include <qurlquery.h>
#include <qvariant.h>
#include <qurl.h>

struct OAuthRequestData {
  QString codeChallenge;
  QString codeVerifier;
  QString state;
  QString endpoint;
  QString clientId;
  QString scope;
  std::map<QString, QVariant> extraParams;

  QUrl url() const {
    QUrlQuery q;

    q.addQueryItem("response_type", "code");
    q.addQueryItem("client_id", clientId);
    q.addQueryItem("redirect_uri", "");
    q.addQueryItem("scope", scope);
    q.addQueryItem("state", state);
    q.addQueryItem("code_challenge", codeChallenge);
    q.addQueryItem("code_challenge_method", "S256");

    for (const auto &[k, v] : extraParams) {
      q.addQueryItem(k, v.toString());
    }

    QUrl url(endpoint);

    url.setQuery(q);

    return url;
  }

  static OAuthRequestData fromUrl(const QUrl &url) {
    OAuthRequestData data;
    QUrlQuery q(url.query());

    data.state = q.queryItemValue("state");

    return data;
  }
};

struct OAuthClient {
  std::optional<QString> id;
  std::optional<QString> description;
  QString name;
  ImageURL icon;

  static OAuthClient fromProto(const proto::ext::oauth::PKCEClientOptions &opts) {
    OAuthClient client;

    client.id = opts.id().c_str();
    client.name = opts.name().c_str();
    client.icon = ImageURL::builtin("omnicast");
    client.description = opts.description().c_str();

    return client;
  }
};

struct OAuthResponseData {
  QString code;
};

using OAuthResponse = std::expected<OAuthResponseData, QString>;

struct OAuthRequest {
  QPromise<OAuthResponse> promise;
};

class OAuthService {

public:
  OAuthService(OmniDatabase &db) : m_tokenStore(db) {}

  OAuth::TokenStore &store() { return m_tokenStore; }

  QFuture<OAuthResponse> authorize(const QString &state) {
    QPromise<OAuthResponse> promise;
    auto future = promise.future();
    auto request = std::make_unique<OAuthRequest>(OAuthRequest{.promise = std::move(promise)});

    m_requests.insert({state, std::move(request)});

    return future;
  }

  void cancelRequest(const QString &state) {
    if (auto it = m_requests.find(state); it != m_requests.end()) {
      auto req = it->second.get();

      req->promise.addResult(std::unexpected("Request was canceled"));
      req->promise.finish();
      m_requests.erase(it);
      return;
    }

    qCritical() << "No oauth request for state" << state;
  }

  void fullfillRequest(const QString &state, const QString &code) {
    if (auto it = m_requests.find(state); it != m_requests.end()) {
      auto req = it->second.get();

      req->promise.addResult(OAuthResponseData{.code = code});
      req->promise.finish();
      m_requests.erase(it);

      return;
    }

    qCritical() << "No oauth request for state" << state;
  }

private:
  std::unordered_map<QString, std::unique_ptr<OAuthRequest>> m_requests;
  OAuth::TokenStore m_tokenStore;
};
