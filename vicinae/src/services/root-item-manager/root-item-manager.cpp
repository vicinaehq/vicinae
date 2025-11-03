#include "root-item-manager.hpp"
#include "root-search.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "utils.hpp"
#include <qlogging.h>
#include <ranges>

std::vector<std::shared_ptr<RootItem>> RootItemManager::fallbackItems() const {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &item : allItems()) {
    if (isFallback(item->uniqueId())) { items.emplace_back(item); }
  }

  std::ranges::sort(items, [&](auto &&a, auto &&b) {
    return itemMetadata(a->uniqueId()).fallbackPosition < itemMetadata(b->uniqueId()).fallbackPosition;
  });

  return items;
}

RootItemMetadata RootItemManager::loadMetadata(const QString &id) {
  RootItemMetadata item;
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		SELECT
			enabled, fallback_position, alias, rank_visit_count, rank_last_visited_at, provider_id, favorite
		FROM
			root_provider_item
		WHERE id = :id
	)");
  query.bindValue(":id", id);

  if (!query.exec()) {
    qCritical() << "Failed to load item metadata for" << id << query.lastError();
    return {};
  }

  if (!query.next()) { return {}; };

  item.isEnabled = query.value(0).toBool();
  item.fallbackPosition = query.value(1).toInt();
  item.alias = query.value(2).toString();
  item.visitCount = query.value(3).toInt();
  item.lastVisitedAt = std::chrono::system_clock::from_time_t(query.value(4).toULongLong());
  item.providerId = query.value(5).toString();
  item.favorite = query.value(6).toBool();

  return item;
}

RootItem *RootItemManager::findItemById(const QString &id) const {
  auto it = std::ranges::find_if(m_items, [&](auto &&v) { return v->uniqueId() == id; });

  if (it != m_items.end()) return it->get();

  return nullptr;
}

RootProvider *RootItemManager::findProviderById(const QString &id) const {
  auto it = std::ranges::find_if(m_providers, [&](auto &&provider) { return provider->uniqueId() == id; });

  if (it == m_providers.end()) return nullptr;

  return it->get();
}

void RootItemManager::updateIndex() {
  static bool isReloading = false;

  if (isReloading) {
    qWarning() << "nested reloadProviders() detected, ignoring.";
    return;
  }

  isReloading = true;

  if (!m_db.db().transaction()) {
    qWarning() << "Failed to create transaction" << m_db.db().lastError();
    return;
  }

  m_items.clear();

  for (const auto &provider : m_providers) {
    auto items = provider->loadItems();

    for (const auto &item : items) {
      upsertItem(provider->uniqueId(), *item.get());
    }

    m_items.insert(m_items.end(), items.begin(), items.end());
  }

  if (!m_db.db().commit()) { qWarning() << "Failed to commit transaction" << m_db.db().lastError(); }

  isReloading = false;
  emit itemsChanged();
}

bool RootItemManager::upsertItem(const QString &providerId, const RootItem &item) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO 
			root_provider_item (id, provider_id, enabled) 
		VALUES (:id, :provider_id, :enabled) 
		ON CONFLICT(id) DO NOTHING
	)");
  query.bindValue(":id", item.uniqueId());
  query.bindValue(":provider_id", providerId);
  query.bindValue(":enabled", !item.isDefaultDisabled());

  if (!query.exec()) {
    qCritical() << "Failed to upsert provider with id" << item.uniqueId() << query.lastError();
    return false;
  }

  m_metadata[item.uniqueId()] = loadMetadata(item.uniqueId());
  item.preferenceValuesChanged(getItemPreferenceValues(item.uniqueId()));

  return true;
}

bool RootItemManager::upsertProvider(const RootProvider &provider) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO 
			root_provider (id) 
		VALUES (:id) 
		ON CONFLICT(id) 
		DO NOTHING 
	)");
  query.bindValue(":id", provider.uniqueId());

  if (!query.exec()) {
    qWarning() << "Failed to upsert provider with id" << provider.uniqueId() << query.lastError();
    return false;
  }

  return true;
}

std::vector<std::shared_ptr<RootItem>>
RootItemManager::prefixSearch(const QString &query, const RootItemPrefixSearchOptions &opts) {
  RootSearcher searcher(m_metadata);
  std::vector<RootSearcher::ScoredItem> results;

  if (!opts.includeDisabled) {
    std::vector<std::shared_ptr<RootItem>> candidates;

    for (const auto &item : m_items) {
      if (itemMetadata(item->uniqueId()).isEnabled) { candidates.emplace_back(item); }
    }

    results = searcher.search(candidates, query);
  } else {
    results = searcher.search(m_items, query);
  }

  std::ranges::sort(results, [this, &query](const auto &a, const auto &b) {
    auto ameta = itemMetadata(a.item->uniqueId());
    auto bmeta = itemMetadata(b.item->uniqueId());
    bool aHasAlias = !ameta.alias.isEmpty() && ameta.alias.startsWith(query, Qt::CaseInsensitive);
    bool bHasAlias = !bmeta.alias.isEmpty() && bmeta.alias.startsWith(query, Qt::CaseInsensitive);

    if (aHasAlias - bHasAlias) { return aHasAlias > bHasAlias; }

    auto ascore = a.score * computeScore(ameta, a.item->baseScoreWeight());
    auto bscore = b.score * computeScore(bmeta, b.item->baseScoreWeight());

    return ascore > bscore;
  });

  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(results.size());
  for (const auto &result : results) {
    items.emplace_back(result.item);
  }

  return items;
}

bool RootItemManager::setItemEnabled(const QString &id, bool value) {
  auto it = std::ranges::find_if(m_items, [&id](const auto &item) { return item->uniqueId() == id; });

  if (it == m_items.end()) {
    qCritical() << "No such item to enable" << id;
    return false;
  }

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE root_provider_item SET enabled = :enabled WHERE id = :id");
  query.bindValue(":enabled", value);
  query.bindValue(":id", id);

  if (!query.exec()) {
    qDebug() << "Failed to update item" << query.lastError();
    return false;
  }

  auto metadata = itemMetadata((*it)->uniqueId());

  metadata.isEnabled = value;
  m_metadata[(*it)->uniqueId()] = metadata;

  return true;
}

bool RootItemManager::setProviderPreferenceValues(const QString &id, const QJsonObject &preferences) {
  auto provider = findProviderById(id);

  if (!provider) return false;

  auto query = m_db.createQuery();
  QJsonDocument json;

  json.setObject(preferences);
  query.prepare("UPDATE root_provider SET preference_values = :preferences WHERE id = :id");
  query.bindValue(":preferences", json.toJson());
  query.bindValue(":id", id);

  if (!query.exec()) {
    qDebug() << "setRepositoryPreferenceValues:" << query.lastError();
    return false;
  }

  provider->preferencesChanged(preferences);

  return true;
}

bool RootItemManager::setItemPreferenceValues(const QString &id, const QJsonObject &preferences) {
  auto query = m_db.createQuery();
  QJsonDocument json;
  RootItem *item = findItemById(id);

  if (!item) return false;

  if (!query.prepare("UPDATE root_provider_item SET preference_values = :preferences WHERE id = :id")) {
    qDebug() << "Failed to prepare update preference query" << query.lastError().driverText();
    return false;
  }

  json.setObject(preferences);
  query.bindValue(":preferences", json.toJson());
  query.bindValue(":id", id);

  if (!query.exec()) {
    qDebug() << "setCommandPreferenceValues:" << query.lastError().driverText();
    return false;
  }

  item->preferenceValuesChanged(preferences);

  return true;
}

void RootItemManager::setPreferenceValues(const QString &id, const QJsonObject &preferences) {
  auto item = findItemById(id);

  if (!item) {
    qCritical() << "setPreferenceValues: no item with id" << id;
    return;
  }

  QJsonObject extensionPreferences, commandPreferences;
  auto metadata = itemMetadata(id);
  auto provider = findProviderById(metadata.providerId);

  if (!provider) {
    qCritical() << "no provider for id" << metadata.providerId;
    return;
  }

  for (const auto &preference : getMergedItemPreferences(id)) {
    auto prefId = preference.name();
    bool isRepositoryPreference = false;

    if (provider) {
      for (const auto &repoPref : provider->preferences()) {
        if (repoPref.name() == prefId) {
          extensionPreferences[prefId] = preferences.value(prefId);
          isRepositoryPreference = true;
          break;
        }
      }
    }

    if (!isRepositoryPreference && preferences.contains(prefId)) {
      commandPreferences[prefId] = preferences.value(prefId);
    }
  }

  m_db.db().transaction();
  if (provider) { setProviderPreferenceValues(provider->uniqueId(), extensionPreferences); }
  setItemPreferenceValues(id, commandPreferences);
  m_db.db().commit();
}

bool RootItemManager::setAlias(const QString &id, const QString &alias) {
  auto it = std::ranges::find_if(m_items, [&id](const auto &item) { return item->uniqueId() == id; });

  if (it == m_items.end()) {
    qCritical() << "setAlias: no item with id " << id;
    return false;
  }

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE root_provider_item SET alias = :alias WHERE id = :id");
  query.bindValue(":alias", alias);
  query.bindValue(":id", id);

  if (!query.exec()) {
    qDebug() << "Failed to update item" << query.lastError();
    return false;
  }

  auto metadata = itemMetadata(id);

  metadata.alias = alias;
  m_metadata[id] = metadata;

  return true;
}

bool RootItemManager::clearAlias(const QString &id) { return setAlias(id, ""); }

QJsonObject RootItemManager::getProviderPreferenceValues(const QString &id) const {
  auto provider = findProviderById(id);

  if (!provider) {
    qWarning() << "No provider with id" << id;
    return {};
  }

  auto query = m_db.createQuery();

  query.prepare(R"(
		SELECT 
			provider.preference_values as preference_values 
		FROM 
			root_provider as provider
		WHERE
			provider.id = :id
	)");
  query.addBindValue(id);

  if (!query.exec()) {
    qDebug() << "Failed to get preference values for provider with ID" << id << query.lastError();
    return {};
  }

  if (!query.next()) {
    qDebug() << "No results";
    return {};
  }
  auto rawJson = query.value(0).toString();
  auto json = QJsonDocument::fromJson(rawJson.toUtf8()).object();

  for (const auto &pref : provider->preferences()) {
    auto dflt = pref.defaultValue();

    if (!json.contains(pref.name()) && !dflt.isNull()) { json[pref.name()] = dflt; }
  }

  return json;
}

bool RootItemManager::pruneProvider(const QString &id) {
  auto query = m_db.createQuery();

  query.prepare("DELETE FROM root_provider WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "pruneProvider() failed" << id << query.lastError();
    return false;
  }

  return true;
}

QJsonObject RootItemManager::getItemPreferenceValues(const QString &id) const {
  auto query = m_db.createQuery();
  auto it = std::ranges::find_if(m_items, [&id](const auto &item) { return item->uniqueId() == id; });

  if (it == m_items.end()) { return {}; }

  auto &item = *it;

  query.prepare(R"(
		SELECT 
			item.preference_values as preference_values 
		FROM 
			root_provider_item as item
		WHERE
			item.id = :id
	)");
  query.addBindValue(id);

  if (!query.exec()) {
    qDebug() << "Failed to get preference values for provider with ID" << id << query.lastError();
    return {};
  }

  if (!query.next()) {
    qDebug() << "No results";
    return {};
  }
  auto rawJson = query.value(0).toString();
  auto json = QJsonDocument::fromJson(rawJson.toUtf8());
  QJsonObject values = json.object();

  for (const auto &preference : item->preferences()) {
    QJsonValue defaultValue = preference.defaultValue();
    if (!values.contains(preference.name()) && !defaultValue.isNull()) {
      values[preference.name()] = defaultValue;
    }
  }

  return values;
}

std::vector<Preference> RootItemManager::getMergedItemPreferences(const QString &rootItemId) const {
  auto metadata = itemMetadata(rootItemId);
  auto provider = findProviderById(metadata.providerId);
  auto item = findItemById(rootItemId);

  if (!provider || !item) return {};

  auto pprefs = provider->preferences();
  auto iprefs = item->preferences();

  pprefs.insert(pprefs.end(), iprefs.begin(), iprefs.end());

  return pprefs;
}

QJsonObject RootItemManager::getPreferenceValues(const QString &id) const {
  auto query = m_db.createQuery();
  auto it = std::ranges::find_if(m_items, [&](auto &&item) { return item->uniqueId() == id; });

  if (it == m_items.end()) {
    qWarning() << "No item with id" << id;
    return {};
  }

  auto &item = *it;

  query.prepare(R"(
		SELECT 
			json_patch(provider.preference_values, item.preference_values) as preference_values 
		FROM 
			root_provider_item as item
		LEFT JOIN
			root_provider as provider
		ON 
			provider.id = item.provider_id
		WHERE
			item.id = :id
	)");
  query.addBindValue(id);

  if (!query.exec()) {
    qDebug() << "Failed to get preference values for command with ID" << id << query.lastError();
    return {};
  }

  if (!query.next()) {
    qWarning() << "No results";
    return {};
  }
  auto rawJson = query.value(0).toString();

  auto json = QJsonDocument::fromJson(rawJson.toUtf8());
  auto preferenceValues = json.object();

  for (auto pref : getMergedItemPreferences(id)) {
    auto dflt = pref.defaultValue();
    auto value = preferenceValues.value(pref.name());
    auto hasValue = !value.isUndefined() && !value.isNull();

    if (!hasValue && !dflt.isUndefined()) { preferenceValues[pref.name()] = dflt; }
  }

  return preferenceValues;
}

RootItemMetadata RootItemManager::itemMetadata(const QString &id) const {
  if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }

  return {};
}

int RootItemManager::maxFallbackPosition() {
  int max = -1;

  for (const auto &[k, v] : m_metadata) {
    if (v.fallbackPosition > max) max = v.fallbackPosition;
  }

  return max;
}

bool RootItemManager::isFallback(const QString &id) const { return itemMetadata(id).fallbackPosition != -1; }

bool RootItemManager::moveFallbackDown(const QString &id) {
  QSqlQuery query = m_db.createQuery();
  int pos = m_metadata[id].fallbackPosition;

  query.prepare(
      "SELECT id FROM root_provider_item WHERE fallback_position > :pos ORDER BY fallback_position ASC");
  query.bindValue(":pos", pos);

  if (!query.exec()) {
    qWarning() << "Failed to exec" << query.lastError();
    return false;
  }

  QSqlQuery updateQuery = m_db.createQuery();

  updateQuery.prepare("UPDATE root_provider_item SET fallback_position = :pos WHERE id = :id");

  int idx = 0;

  while (query.next()) {
    QString nextId = query.value(0).toString();

    updateQuery.bindValue(":pos", pos);
    updateQuery.bindValue(":id", nextId);

    if (!updateQuery.exec()) {
      qWarning() << "Failed to exec" << query.lastError();
      return false;
    }

    m_metadata[nextId].fallbackPosition = pos;

    if (idx == 0) {
      updateQuery.bindValue(":pos", ++pos);
      updateQuery.bindValue(":id", id);

      if (!updateQuery.exec()) {
        qWarning() << "Failed to exec" << query.lastError();
        return false;
      }

      m_metadata[id].fallbackPosition = pos;
    }

    ++idx;
    ++pos;
  }

  emit fallbackOrderChanged(id);

  return true;
}

bool RootItemManager::disableFallback(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("SELECT id, fallback_position FROM root_provider_item WHERE fallback_position >= 0 ORDER BY "
                "fallback_position");
  query.exec();

  QSqlQuery updateQuery = m_db.createQuery();

  updateQuery.prepare("UPDATE root_provider_item SET fallback_position = :pos WHERE id = :id");
  updateQuery.bindValue(":pos", -1);
  updateQuery.bindValue(":id", id);
  updateQuery.exec();
  m_metadata[id].fallbackPosition = -1;

  int normalizedPos = 0;

  while (query.next()) {
    QString nextId = query.value(0).toString();

    if (nextId == id) continue;

    updateQuery.bindValue(":pos", normalizedPos);
    updateQuery.bindValue(":id", nextId);
    updateQuery.exec();
    m_metadata[nextId].fallbackPosition = normalizedPos;
    ++normalizedPos;
  }

  emit fallbackDisabled(id);

  return true;
}

bool RootItemManager::enableFallback(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("SELECT id, fallback_position FROM root_provider_item WHERE fallback_position >= 0 ORDER BY "
                "fallback_position");
  query.exec();

  QSqlQuery updateQuery = m_db.createQuery();

  updateQuery.prepare("UPDATE root_provider_item SET fallback_position = :pos WHERE id = :id");
  updateQuery.bindValue(":pos", 0);
  updateQuery.bindValue(":id", id);

  if (!updateQuery.exec()) {
    qWarning() << "failed to update" << updateQuery.lastError();
    return false;
  }

  m_metadata[id].fallbackPosition = 0;

  int normalizedPos = 1;

  while (query.next()) {
    QString nextId = query.value(0).toString();

    if (nextId == id) continue;

    updateQuery.bindValue(":pos", normalizedPos);
    updateQuery.bindValue(":id", nextId);
    updateQuery.exec();
    m_metadata[nextId].fallbackPosition = normalizedPos;
    ++normalizedPos;
  }

  emit fallbackEnabled(id);

  return true;
}

bool RootItemManager::moveFallbackUp(const QString &id) {
  QSqlQuery query = m_db.createQuery();
  int pos = m_metadata[id].fallbackPosition;

  if (pos <= 0) return false;

  query.prepare("SELECT id FROM root_provider_item WHERE fallback_position >= 0 AND fallback_position < :pos "
                "ORDER BY fallback_position DESC");
  query.bindValue(":pos", pos);

  if (!query.exec()) {
    qWarning() << "Failed to exec" << query.lastError();
    return false;
  }

  QSqlQuery updateQuery = m_db.createQuery();

  updateQuery.prepare("UPDATE root_provider_item SET fallback_position = :pos WHERE id = :id");

  int idx = 0;

  while (query.next()) {
    QString nextId = query.value(0).toString();

    updateQuery.bindValue(":pos", pos);
    updateQuery.bindValue(":id", nextId);

    if (!updateQuery.exec()) {
      qWarning() << "Failed to exec" << query.lastError();
      return false;
    }

    m_metadata[nextId].fallbackPosition = pos;

    if (idx == 0) {
      updateQuery.bindValue(":pos", --pos);
      updateQuery.bindValue(":id", id);

      if (!updateQuery.exec()) {
        qWarning() << "Failed to exec" << query.lastError();
        return false;
      }

      m_metadata[id].fallbackPosition = pos;
    }

    ++idx;
    --pos;
  }

  emit fallbackOrderChanged(id);

  return true;
}

bool RootItemManager::setItemAsFavorite(const QString &itemId, bool value) {
  auto query = m_db.createQuery();

  query.prepare(R"(
		UPDATE root_provider_item 
		SET favorite = :favorite
		WHERE id = :id
	)");
  query.addBindValue(value);
  query.addBindValue(itemId);

  if (!query.exec()) {
    qCritical() << "Failed to set item as favorite" << itemId << value;
    return false;
  }

  m_metadata[itemId].favorite = value;
  emit itemFavoriteChanged(itemId, value);

  return true;
}

double RootItemManager::computeRecencyScore(const RootItemMetadata &meta) const {
  if (!meta.lastVisitedAt) return 0.1;

  auto now = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::days>(now - *meta.lastVisitedAt).count();
  auto hoursSince = std::chrono::duration_cast<std::chrono::hours>(now - *meta.lastVisitedAt).count() / 24.0;

  if (hoursSince < 1) return 2.0;
  if (hoursSince < 6) return 1.5;

  return std::exp(-hoursSince / 30.0);
}

double RootItemManager::computeScore(const RootItemMetadata &meta, int weight) const {
  double frequencyScore = std::log(meta.visitCount + 1);
  double recencyScore = computeRecencyScore(meta);

  return (frequencyScore + recencyScore) * weight;
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::queryFavorites(int limit) {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &item : m_items) {
    RootItemMetadata meta = itemMetadata(item->uniqueId());

    if (meta.isEnabled && meta.favorite) { items.emplace_back(item); }
  }

  std::ranges::sort(items, [this](const auto &a, const auto &b) {
    RootItemMetadata ameta = itemMetadata(a->uniqueId());
    RootItemMetadata bmeta = itemMetadata(b->uniqueId());
    auto ascore = computeScore(ameta, a->baseScoreWeight());
    auto bscore = computeScore(ameta, b->baseScoreWeight());

    return ameta.visitCount > bmeta.visitCount;
  });

  if (items.size() > limit) { items.resize(limit); }

  return items;
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::querySuggestions(int limit) {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &item : m_items) {
    RootItemMetadata meta = itemMetadata(item->uniqueId());

    if (meta.isEnabled && meta.visitCount > 0) { items.emplace_back(item); }
  }

  std::ranges::sort(items, [this](const auto &a, const auto &b) {
    RootItemMetadata ameta = itemMetadata(a->uniqueId());
    RootItemMetadata bmeta = itemMetadata(b->uniqueId());
    auto ascore = computeScore(ameta, a->baseScoreWeight());
    auto bscore = computeScore(ameta, b->baseScoreWeight());

    return ameta.visitCount > bmeta.visitCount;
  });

  if (items.size() > limit) { items.resize(limit); }

  return items;
}

bool RootItemManager::resetRanking(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		UPDATE root_provider_item 
		SET 
			rank_visit_count = 0,
			rank_last_visited_at = NULL
		WHERE id = :id
	)");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to reset ranking" << query.lastError();
    return false;
  }

  RootItemMetadata &metadata = m_metadata[id];

  metadata.lastVisitedAt = std::nullopt;
  metadata.visitCount = 0;
  emit itemRankingReset(id);

  return true;
}

bool RootItemManager::registerVisit(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		UPDATE root_provider_item 
		SET 
			visit_count = visit_count + 1,
			rank_visit_count = rank_visit_count + 1,
			last_visited_at = unixepoch(),
			rank_last_visited_at = unixepoch()
		WHERE id = :id
		RETURNING rank_visit_count, rank_last_visited_at, visit_count, last_visited_at
	)");
  query.bindValue(":id", id);

  if (!query.exec() || !query.next()) {
    qDebug() << "Failed to update item" << query.lastError();
    return false;
  }

  RootItemMetadata &meta = m_metadata[id];

  meta.visitCount = query.value(0).toInt();
  meta.lastVisitedAt = std::chrono::system_clock::from_time_t(query.value(1).toULongLong());

  return true;
}

QString RootItemManager::getItemProviderId(const QString &id) {
  auto metadata = itemMetadata(id);

  return metadata.providerId;
}

bool RootItemManager::setProviderEnabled(const QString &providerId, bool value) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		UPDATE root_provider_item
		SET enabled = :enabled
		WHERE provider_id = :provider_id
	)");
  query.bindValue(":enabled", value);
  query.bindValue(":provider_id", providerId);

  if (!query.exec()) {
    qDebug() << "Failed to update item" << query.lastError();
    return false;
  }

  for (auto &[id, metadata] : m_metadata) {
    if (providerId == metadata.providerId) { metadata.isEnabled = value; }
  }

  m_provider_metadata[providerId].enabled = value;

  return true;
}

bool RootItemManager::disableItem(const QString &id) { return setItemEnabled(id, false); }

bool RootItemManager::enableItem(const QString &id) { return setItemEnabled(id, true); }

std::vector<RootProvider *> RootItemManager::providers() const {
  std::vector<RootProvider *> providers;

  for (const auto &provider : m_providers) {
    providers.emplace_back(provider.get());
  }

  return providers;
}

void RootItemManager::uninstallProvider(const QString &id) {
  if (pruneProvider(id)) { unloadProvider(id); }
}

std::vector<ExtensionRootProvider *> RootItemManager::extensions() const {
  std::vector<ExtensionRootProvider *> providers;

  for (const auto &provider : m_providers) {
    if (provider->isExtension()) providers.emplace_back(static_cast<ExtensionRootProvider *>(provider.get()));
  }

  return providers;
}

void RootItemManager::unloadProvider(const QString &id) {
  auto it = std::ranges::find_if(m_providers, [&](auto &&p) { return p->uniqueId() == id; });

  if (it == m_providers.end()) return;

  m_providers.erase(it);
}

void RootItemManager::loadProvider(std::unique_ptr<RootProvider> provider) {
  if (!upsertProvider(*provider)) {
    qWarning() << "Failed to upsert provider";
    return;
  };

  auto pred = [&](auto &&p) { return p->uniqueId() == provider->uniqueId(); };
  auto it = std::ranges::find_if(m_providers, pred);

  if (it != m_providers.end()) {
    *it = std::move(provider);
    return;
  }

  auto ptr = provider.get();

  m_providers.emplace_back(std::move(provider));
  auto preferenceValues = getProviderPreferenceValues(ptr->uniqueId());

  if (auto patched = ptr->patchPreferences(preferenceValues)) {
    setProviderPreferenceValues(ptr->uniqueId(), patched.value());
    preferenceValues = patched.value();
  }

  ptr->preferencesChanged(preferenceValues);
  ptr->initialized(preferenceValues);
  connect(ptr, &RootProvider::itemsChanged, this, [this]() { updateIndex(); });
}

RootProvider *RootItemManager::provider(const QString &id) const {
  auto it = std::ranges::find_if(m_providers, [&id](const auto &p) { return id == p->uniqueId(); });

  if (it != m_providers.end()) return it->get();

  return nullptr;
}
