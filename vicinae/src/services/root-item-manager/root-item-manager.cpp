#include "root-item-manager.hpp"
#include "root-search.hpp"
#include <bits/chrono.h>
#include <qlogging.h>
#include <ranges>

RootItemMetadata RootItemManager::loadMetadata(const QString &id) {
  RootItemMetadata item;
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		SELECT
			enabled, fallback, fallback_position, alias, rank_visit_count, rank_last_visited_at, provider_id, favorite
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
  item.isFallback = query.value(1).toBool();
  item.fallbackPosition = query.value(2).toInt();
  item.alias = query.value(3).toString();
  item.visitCount = query.value(4).toInt();
  item.lastVisitedAt = std::chrono::system_clock::from_time_t(query.value(5).toULongLong());
  item.providerId = query.value(6).toString();
  item.favorite = query.value(7).toBool();

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

void RootItemManager::reloadProviders() {
  static bool isReloading = false;

  if (isReloading) {
    qWarning() << "nested reloadProviders() detected, ignoring.";
    return;
  }

  m_items.clear();
  isReloading = true;

  for (const auto &provider : m_providers) {
    auto items = provider->loadItems();

    if (!upsertProvider(*provider.get())) continue;

    m_items.insert(m_items.end(), items.begin(), items.end());

    std::ranges::for_each(items, [&](const auto &item) { upsertItem(provider->uniqueId(), *item.get()); });

    auto preferences = getProviderPreferenceValues(provider->uniqueId());

    if (preferences.empty()) {
      preferences = provider->generateDefaultPreferences();

      if (!preferences.empty()) { setProviderPreferenceValues(provider->uniqueId(), preferences); }
    }

    // provider->preferencesChanged(preferences);
  }

  isReloading = false;
  emit itemsChanged();
}

bool RootItemManager::upsertItem(const QString &providerId, const RootItem &item) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO 
			root_provider_item (id, provider_id, enabled, fallback) 
		VALUES (:id, :provider_id, :enabled, :fallback) 
		ON CONFLICT(id) DO NOTHING
	)");
  query.bindValue(":id", item.uniqueId());
  query.bindValue(":provider_id", providerId);
  query.bindValue(":enabled", !item.isDefaultDisabled());
  query.bindValue(":fallback", item.isDefaultFallback());

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
    auto candidates =
        m_items |
        std::views::filter([this](auto &&item) { return itemMetadata(item->uniqueId()).isEnabled; }) |
        std::ranges::to<std::vector>();
    results = searcher.search(candidates, query);
  } else {
    results = searcher.search(m_items, query);
  }

  std::ranges::sort(results, [this](const auto &a, const auto &b) {
    auto ameta = itemMetadata(a.item->uniqueId());
    auto bmeta = itemMetadata(b.item->uniqueId());
    auto ascore = a.score * computeScore(ameta, a.item->baseScoreWeight());
    auto bscore = b.score * computeScore(bmeta, b.item->baseScoreWeight());

    return ascore > bscore;
  });

  return results | std::views::transform([](auto &&item) { return item.item; }) |
         std::ranges::to<std::vector>();
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
  qDebug() << "set command prefs for" << id;
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

  qDebug() << "Set alias";

  return true;
}

bool RootItemManager::clearAlias(const QString &id) { return setAlias(id, ""); }

QJsonObject RootItemManager::getProviderPreferenceValues(const QString &id) const {
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
  auto json = QJsonDocument::fromJson(rawJson.toUtf8());

  return json.object();
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

    if (!preferenceValues.contains(pref.name()) && !dflt.isUndefined()) {
      preferenceValues[pref.name()] = dflt;
    }
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

bool RootItemManager::isFallback(const QString &id) { return itemMetadata(id).isFallback; }

bool RootItemManager::disableFallback(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE root_provider_item SET fallback = 0, fallback_position = -1 WHERE id = :id");
  query.bindValue(":id", id);

  if (!query.exec()) {
    qCritical() << "Failed to disable fallback for id" << id;
    return false;
  }

  auto meta = itemMetadata(id);

  meta.isFallback = false;
  m_metadata[id] = meta;
  emit fallbackDisabled(id);

  return true;
}

bool RootItemManager::setFallback(const QString &id, int position) {
  if (!m_db.db().transaction()) { return false; }

  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		UPDATE root_provider_item
		SET fallback_position = fallback_position + 1
		WHERE fallback_position >= :position
	)");
  query.bindValue(":id", id);
  query.bindValue(":position", position);

  if (!query.exec()) {
    qCritical() << "Failed to set fallback" << query.lastError();
    m_db.db().rollback();
    return false;
  }

  query.prepare(R"(
		UPDATE root_provider_item 
		SET fallback = 1, fallback_position = :position 
		WHERE id = :id
	)");
  query.bindValue(":id", id);
  query.bindValue(":position", position);

  if (!query.exec()) {
    qCritical() << "Failed to set fallback" << query.lastError();
    m_db.db().rollback();
    return false;
  }

  if (!m_db.db().commit()) { return false; }

  qDebug() << "updated fallback pos for" << id << position;

  for (auto &pair : m_metadata) {
    if (pair.second.fallbackPosition >= position) pair.second.fallbackPosition += 1;
  }

  auto metadata = itemMetadata(id);

  metadata.isFallback = true;
  metadata.fallbackPosition = position;
  m_metadata[id] = metadata;
  emit fallbackEnabled(id);

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
  auto items = m_items | std::views::transform([this](const std::shared_ptr<RootItem> &item) {
                 RootItemMetadata meta = itemMetadata(item->uniqueId());
                 return std::pair<std::shared_ptr<RootItem>, RootItemMetadata>(item, meta);
               }) |
               std::views::filter([](auto &&item) { return item.second.isEnabled && item.second.favorite; }) |
               std::ranges::to<std::vector>();

  std::ranges::sort(items, [this](const auto &a, const auto &b) {
    const auto &[aitem, ameta] = a;
    const auto &[bitem, bmeta] = b;

    auto ascore = computeScore(ameta, aitem->baseScoreWeight());
    auto bscore = computeScore(ameta, bitem->baseScoreWeight());

    return ameta.visitCount > bmeta.visitCount;
  });

  return items | std::views::take(limit) | std::views::transform([](auto &&a) { return a.first; }) |
         std::ranges::to<std::vector>();
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::querySuggestions(int limit) {
  auto items =
      m_items | std::views::transform([this](const std::shared_ptr<RootItem> &item) {
        RootItemMetadata meta = itemMetadata(item->uniqueId());
        return std::pair<std::shared_ptr<RootItem>, RootItemMetadata>(item, meta);
      }) |
      std::views::filter([](auto &&item) { return item.second.isEnabled && item.second.visitCount > 0; }) |
      std::ranges::to<std::vector>();

  std::ranges::sort(items, [this](const auto &a, const auto &b) {
    const auto &[aitem, ameta] = a;
    const auto &[bitem, bmeta] = b;

    auto ascore = computeScore(ameta, aitem->baseScoreWeight());
    auto bscore = computeScore(bmeta, bitem->baseScoreWeight());

    return ascore > bscore;
  });

  return items | std::views::take(limit) | std::views::transform([](auto &&a) { return a.first; }) |
         std::ranges::to<std::vector>();
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
  return m_providers | std::views::transform([](const auto &p) { return p.get(); }) |
         std::ranges::to<std::vector>();
}

void RootItemManager::removeProvider(const QString &id) {
  pruneProvider(id);
  std::erase_if(m_providers, [&](auto &&p) { return p->uniqueId() == id; });
  reloadProviders();
}

void RootItemManager::addProvider(std::unique_ptr<RootProvider> provider) {
  auto items = provider->loadItems();

  if (!m_db.db().transaction()) { qWarning() << "Failed to start upsert transaction"; }

  if (!upsertProvider(*provider.get())) {
    m_db.db().rollback();
    return;
  }

  m_items.insert(m_items.end(), items.begin(), items.end());

  std::ranges::for_each(items, [&](const auto &item) { upsertItem(provider->uniqueId(), *item.get()); });

  m_db.db().commit();

  auto preferences = getProviderPreferenceValues(provider->uniqueId());

  if (preferences.empty()) {
    preferences = provider->generateDefaultPreferences();

    if (!preferences.empty()) {
      qCritical() << "set default preferences for app" << provider->uniqueId();
      setProviderPreferenceValues(provider->uniqueId(), preferences);
    }
  }

  provider->preferencesChanged(preferences);

  connect(provider.get(), &RootProvider::itemsChanged, this, [this, name = provider->uniqueId()]() {
    qDebug() << "provider" << name << "signals change";
    reloadProviders();
  });
  m_providers.emplace_back(std::move(provider));
  emit itemsChanged();
}

RootProvider *RootItemManager::provider(const QString &id) const {
  auto it = std::ranges::find_if(m_providers, [&id](const auto &p) { return id == p->uniqueId(); });

  if (it != m_providers.end()) return it->get();

  return nullptr;
}
