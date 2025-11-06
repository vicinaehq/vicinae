#include "omni-database.hpp"
#include <cstdint>
#include <qtypes.h>
namespace OAuth {

struct TokenSet {
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
class TokenStore {
public:
  TokenStore(OmniDatabase &db);

  std::optional<TokenSet> getTokenSet(const QString &extensionId,
                                      const std::optional<QString> &providerId = {}) const;
  bool setTokenSet(const SetTokenSetPayload &payload) const;
  bool removeTokenSet(const QString &id, const std::optional<QString> &providerId = {}) const;
  TokenSetList getTokens() const;

private:
  OmniDatabase &m_db;
};
}; // namespace OAuth
