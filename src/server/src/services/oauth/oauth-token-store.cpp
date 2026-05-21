#include "oauth-token-store.hpp"
#include "omni-database.hpp"
#include <qlogging.h>

namespace OAuth {

TokenStore::TokenStore(OmniDatabase &db) : m_db(db) {}

bool TokenStore::setTokenSet(const SetTokenSetPayload &payload) const {
  auto stmt = m_db.db().prepare(R"(
    INSERT INTO oauth_token_set(extension_id, provider_id, access_token, refresh_token, id_token, scope, expires_in, updated_at)
    VALUES (:extension_id, :provider_id, :access_token, :refresh_token, :id_token, :scope, :expires_in, :updated_at)
    ON CONFLICT
    DO UPDATE SET
      access_token = :access_token,
      refresh_token = :refresh_token,
      id_token = :id_token,
      scope = :scope,
      expires_in = :expires_in,
      updated_at = :updated_at
  )");

  stmt.bind(":extension_id", payload.extensionId);
  stmt.bind(":provider_id", payload.providerId.value_or(QString("")));
  stmt.bind(":access_token", payload.accessToken);
  stmt.bind(":refresh_token", payload.refreshToken);
  stmt.bind(":id_token", payload.idToken);
  stmt.bind(":scope", payload.scope);

  if (payload.expiresIn) {
    stmt.bind(":expires_in", static_cast<int64_t>(*payload.expiresIn));
  } else {
    stmt.bindNull(":expires_in");
  }

  stmt.bind(":updated_at", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

  if (!stmt.exec()) {
    qWarning() << "Failed to setTokenSet for extension" << payload.extensionId << stmt.lastError().c_str();
    return false;
  }

  emit setUpdated(payload.extensionId);
  return true;
}

bool TokenStore::removeTokenSet(const QString &extensionId, const std::optional<QString> &providerId) const {
  auto stmt = m_db.db().prepare(
      "DELETE FROM oauth_token_set WHERE extension_id = :extension_id AND provider_id = :provider_id");
  stmt.bind(":extension_id", extensionId);
  stmt.bind(":provider_id", providerId.value_or(QString("")));

  if (!stmt.exec()) {
    qWarning() << "Failed to delete token set with id" << extensionId;
    return false;
  }

  emit setRemoved(extensionId);
  return true;
}

std::optional<TokenSet> TokenStore::getTokenSet(const QString &extensionId,
                                                const std::optional<QString> &providerId) const {
  auto stmt = m_db.db().prepare(R"(
    SELECT access_token, refresh_token, id_token, scope, expires_in, updated_at
    FROM oauth_token_set
    WHERE extension_id = :extension_id AND provider_id = :provider_id
  )");
  stmt.bind(":extension_id", extensionId);
  stmt.bind(":provider_id", providerId.value_or(QString("")));

  if (!stmt.step()) { return {}; }

  TokenSet set;
  set.extensionId = extensionId;
  set.providerId = providerId;
  set.accessToken = stmt.columnQString(0);
  if (!stmt.isNull(1)) { set.refreshToken = stmt.columnQString(1); }
  if (!stmt.isNull(2)) { set.idToken = stmt.columnQString(2); }
  if (!stmt.isNull(3)) { set.scope = stmt.columnQString(3); }
  if (!stmt.isNull(4)) { set.expiresIn = static_cast<unsigned int>(stmt.columnInt(4)); }
  set.updatedAt = static_cast<unsigned int>(stmt.columnInt(5));

  return set;
}

TokenSetList TokenStore::list() const {
  auto stmt = m_db.db().prepare(R"(
    SELECT extension_id, provider_id, access_token, refresh_token, id_token, scope, expires_in, updated_at FROM oauth_token_set
  )");

  TokenSetList sets;

  while (stmt.step()) {
    TokenSet set;
    set.extensionId = stmt.columnQString(0);
    if (auto v = stmt.columnQString(1); !v.isEmpty()) { set.providerId = v; }
    set.accessToken = stmt.columnQString(2);
    if (!stmt.isNull(3)) { set.refreshToken = stmt.columnQString(3); }
    if (!stmt.isNull(4)) { set.idToken = stmt.columnQString(4); }
    if (!stmt.isNull(5)) { set.scope = stmt.columnQString(5); }
    if (!stmt.isNull(6)) { set.expiresIn = static_cast<unsigned int>(stmt.columnInt(6)); }
    set.updatedAt = static_cast<unsigned int>(stmt.columnInt(7));
    sets.emplace_back(set);
  }

  return sets;
}

}; // namespace OAuth
