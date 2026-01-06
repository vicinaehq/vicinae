#include "omni-database.hpp"
#include <cstdint>
#include <qdatetime.h>
#include <qobject.h>
#include <qtypes.h>
#include <ranges>

namespace OAuth {

struct TokenSet {
  std::optional<QDateTime> expirationDate() const {
    return expiresIn.transform(
        [&](uint64_t seconds) { return QDateTime::fromSecsSinceEpoch(updatedAt + seconds); });
  }

  bool isExpired() const {
    if (auto expires = expirationDate()) { return QDateTime::currentDateTime() > expires.value(); }
    return false;
  }

  std::vector<QString> scopes() const {
    return scope
        .transform([](const QString &scope) { return scope.split(' ') | std::ranges::to<std::vector>(); })
        .value_or(std::vector<QString>());
  }

  QString extensionId;
  QString accessToken;
  std::optional<QString> providerId;
  std::optional<QString> refreshToken;
  std::optional<QString> idToken;
  std::optional<QString> scope;
  std::optional<uint64_t> expiresIn;
  uint64_t updatedAt;
};

struct SetTokenSetPayload {
  QString extensionId;
  QString accessToken;
  std::optional<QString> providerId;
  std::optional<QString> refreshToken;
  std::optional<QString> idToken;
  std::optional<QString> scope;
  std::optional<uint64_t> expiresIn;
};

using TokenSetList = std::vector<TokenSet>;

/*
 * Retrieve, set and remove oauth token sets.
 * Mostly used by extensions providing oauth integrations.
 */
class TokenStore : public QObject {
  Q_OBJECT

signals:
  void setRemoved(const QString &extensionId) const;
  void setUpdated(const QString &extensionId) const;

public:
  TokenStore(OmniDatabase &db);

  std::optional<TokenSet> getTokenSet(const QString &extensionId,
                                      const std::optional<QString> &providerId = {}) const;
  bool setTokenSet(const SetTokenSetPayload &payload) const;
  bool removeTokenSet(const QString &id, const std::optional<QString> &providerId = {}) const;
  TokenSetList list() const;

private:
  OmniDatabase &m_db;
};
}; // namespace OAuth
