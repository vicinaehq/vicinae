#include "oauth-token-store.hpp"
#include "omni-database.hpp"
#include <qlogging.h>

namespace OAuth {

TokenStore::TokenStore(OmniDatabase &db) : m_db(db) {}

bool TokenStore::setTokenSet(const SetTokenSetPayload &payload) const {
  auto query = m_db.createQuery();

  query.prepare(R"(
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
  // to handle NULL cases properly
  auto strVar = [](auto &&a) { return QVariant(a); };
  auto intVar = [](auto &&a) { return QVariant(static_cast<quint64>(a)); };

  query.bindValue(":extension_id", payload.extensionId);
  query.bindValue(":provider_id", payload.providerId.value_or(""));
  query.bindValue(":access_token", payload.accessToken);
  query.bindValue(":refresh_token", payload.refreshToken.transform(strVar).value_or({}));
  query.bindValue(":id_token", payload.idToken.transform(strVar).value_or({}));
  query.bindValue(":scope", payload.scope.transform(strVar).value_or({}));
  query.bindValue(":expires_in", payload.expiresIn.transform(intVar).value_or({}));
  query.bindValue(":updated_at", QDateTime::currentSecsSinceEpoch());

  if (!query.exec()) {
    qWarning() << "Failed to setTokenSet for extension" << payload.extensionId << query.lastError();
    return false;
  }

  return true;
}

bool TokenStore::removeTokenSet(const QString &extensionId, const std::optional<QString> &providerId) const {
  auto query = m_db.createQuery();

  query.prepare(
      "DELETE FROM oauth_token_set WHERE extension_id = :extension_id AND provider_id = :provider_id");
  query.bindValue(":extension_id", extensionId);
  query.bindValue(":provider_id", providerId.value_or(""));

  if (!query.exec()) {
    qWarning() << "Failed to delete token set with id" << extensionId;
    return false;
  }

  return true;
}

std::optional<TokenSet> TokenStore::getTokenSet(const QString &extensionId,
                                                const std::optional<QString> &providerId) const {
  auto query = m_db.createQuery();

  query.prepare(R"(
		SELECT access_token, refresh_token, id_token, scope, expires_in, updated_at
		FROM oauth_token_set
		WHERE extension_id = :extension_id AND provider_id = :provider_id
	)");
  query.bindValue(":extension_id", extensionId);
  query.bindValue(":provider_id", providerId.value_or(""));

  if (!query.exec()) {
    qWarning() << "Failed to getTokenSet" << query.lastError();
    return {};
  }
  if (!query.next()) { return {}; }

  TokenSet set;
  set.extensionId = extensionId;
  set.providerId = providerId;
  set.accessToken = query.value(0).toString();
  if (auto v = query.value(1); !v.isNull()) { set.refreshToken = v.toString(); }
  if (auto v = query.value(2); !v.isNull()) { set.idToken = v.toString(); }
  if (auto v = query.value(3); !v.isNull()) { set.scope = v.toString(); }
  if (auto v = query.value(4); !v.isNull()) { set.expiresIn = v.toUInt(); }
  set.updatedAt = query.value(5).toUInt();

  return set;
}

TokenSetList TokenStore::getTokens() const { return {}; }

}; // namespace OAuth
