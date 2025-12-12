#include "root-item-manager.hpp"
#include "common.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include <qjsonobject.h>
#include <ranges>
#include "lib/fzf.hpp"
#include <algorithm>
#include <qlogging.h>
#include <qtconcurrentfilter.h>
#include "config/config.hpp"
#include <ranges>

RootItemManager::RootItemManager(config::Manager &cfg, OmniDatabase &db) : m_cfg(cfg), m_db(db) {
  connect(&cfg, &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            for (const auto &[id, provider] : next.providers) {
              for (const auto &[itemId, item] : provider.items) {
                EntrypointId entrypoint{id, itemId};
                m_metadata[entrypoint] =
                    RootItemMetadata{.isEnabled = item.enabled.value_or(true), .alias = item.alias};
              }
            }
          });
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::fallbackItems() const {
  std::vector<std::shared_ptr<RootItem>> items;

  for (const auto &item : allItems()) {
    // if (isFallback(item.item->uniqueId())) { items.emplace_back(item.item); }
  }

  /*
  std::ranges::sort(items, [&](auto &&a, auto &&b) {
    return itemMetadata(a->uniqueId()).fallbackPosition < itemMetadata(b->uniqueId()).fallbackPosition;
  });
  */

  return items;
}

/*
RootItemMetadata RootItemManager::loadMetadata(const EntrypointId &id) {
RootItemMetadata &item = m_metadata[id];
QSqlQuery query = m_db.createQuery();

query.prepare(R"(
        SELECT
                rank_visit_count, rank_last_visited_at, provider_id
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

item.visitCount = query.value(0).toInt();
item.lastVisitedAt = std::chrono::system_clock::from_time_t(query.value(1).toULongLong());
item.providerId = query.value(2).toString().toStdString();

return item;

return {};
}
*/

RootItem *RootItemManager::findItemById(const EntrypointId &id) const {
  for (const auto &item : m_items) {
    if (item.item->uniqueId() == id) { return item.item.get(); }
  }
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
      SearchableRootItem sitem;

      sitem.item = item;
      sitem.title = item->displayName().toStdString();
      sitem.subtitle = item->subtitle().toStdString();
      sitem.keywords = item->keywords() | std::views::transform([](auto &&s) { return s.toStdString(); }) |
                       std::ranges::to<std::vector>();
      sitem.meta = &m_metadata[item->uniqueId()];
      sitem.meta->providerId = provider->uniqueId().toStdString();
      m_items.emplace_back(sitem);
    }
  }

  if (!m_db.db().commit()) { qWarning() << "Failed to commit transaction" << m_db.db().lastError(); }

  m_scoredItems.reserve(m_items.size());
  isReloading = false;
  emit itemsChanged();
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

float RootItemManager::SearchableRootItem::fuzzyScore(std::string_view pattern) const {
  using WS = fzf::WeightedString;
  std::string alias = meta->alias.value_or("");
  std::initializer_list<WS> ss = {{title, 1.0f}, {subtitle, 0.6f}, {alias, 1.0f}};
  auto kws = keywords | std::views::transform([](auto &&kw) { return WS{kw, 0.3f}; });
  auto strs = std::views::concat(ss, kws);
  float score = pattern.empty() ? 1 : fzf::defaultMatcher.fuzzy_match_v2_score_query(strs, pattern, false);
  double frequencyScore = std::log(1 + meta->visitCount * 0.1);
  float frequencyWeight = 0.2;

  // TODO: add recency support

  return score * (1 + frequencyScore * frequencyWeight);
}

std::span<RootItemManager::ScoredItem> RootItemManager::search(const QString &query,
                                                               const RootItemPrefixSearchOptions &opts) {
  // Timer timer;
  std::string pattern = query.toStdString();
  std::string_view patternView = pattern;

  m_scoredItems.clear();

  for (auto &item : m_items) {
    if (!item.meta->isEnabled && !opts.includeDisabled) continue;
    // if (item.meta->favorite && !opts.includeFavorites) continue;
    double fuzzyScore = item.fuzzyScore(patternView);

    if (!fuzzyScore) { continue; }

    m_scoredItems.emplace_back(ScoredItem{
        .alias = item.meta->alias.value_or(""), .meta = item.meta, .score = fuzzyScore, .item = item.item});
  }

  // we need stable sort to avoid flickering when updating quickly
  std::ranges::stable_sort(m_scoredItems, [&](const auto &a, const auto &b) {
    if (opts.prioritizeAliased) {
      bool aa = !a.alias.empty() && a.alias.starts_with(pattern);
      bool ab = !b.alias.empty() && b.alias.starts_with(pattern);
      // always prioritize matching aliases over score
      if (aa - ab) { return aa > ab; }
    }

    return a.score > b.score;
  });

  // timer.time("root search");

  return m_scoredItems;
}

bool RootItemManager::setItemEnabled(const EntrypointId &id, bool value) {
  auto merged = m_cfg.mergeEntrypointWithUser(id, {.enabled = value});
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

bool RootItemManager::setItemPreferenceValues(const EntrypointId &id, const QJsonObject &preferences) {
  auto query = m_db.createQuery();
  QJsonDocument json;
  RootItem *item = findItemById(id);

  if (!item) return false;

  m_metadata[id].preferences = preferences;
  // m_cfg.providers[id.provider].items[id.entrypoint].preferences = preferences;

  // m_cfg.print();

  item->preferenceValuesChanged(preferences);

  return true;
}

void RootItemManager::setPreferenceValues(const EntrypointId &id, const QJsonObject &preferences) {
  auto item = findItemById(id);
  auto prvd = provider(id.provider);

  QJsonObject providerPreferenceValues;
  QJsonObject entrypointPreferenceValues;

  if (!item) {
    // qCritical() << "setPreferenceValues: no item with id" << id
    return;
  }

  for (const auto &pref : prvd->preferences()) {
    if (preferences.contains(pref.name())) {
      providerPreferenceValues[pref.name()] = preferences.value(pref.name());
    }
  }

  for (const auto &pref : item->preferences()) {
    if (preferences.contains(pref.name())) {
      entrypointPreferenceValues[pref.name()] = preferences.value(pref.name());
    }
  }

  if (!providerPreferenceValues.empty()) {
    // m_cfg.providers[id.provider].preferences = providerPreferenceValues;
  }

  if (!entrypointPreferenceValues.empty()) {
    // m_cfg.providers[id.provider].items[id.entrypoint].preferences = entrypointPreferenceValues;
  }

  // m_cfg.print();

  qWarning() << "setPreferences not yet implemented";
}

bool RootItemManager::setAlias(const EntrypointId &id, std::string_view alias) {
  auto &meta = m_metadata[id];

  m_metadata[id].alias = alias;
  m_cfg.mergeEntrypointWithUser(id, {.alias = std::string{alias}});

  return true;
}

bool RootItemManager::clearAlias(const EntrypointId &id) { return setAlias(id, ""); }

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

QJsonObject RootItemManager::getItemPreferenceValues(const EntrypointId &id) const {
  auto item = findItemById(id);

  if (!item) { return {}; }

  QJsonObject values = itemMetadata(id).preferences.value_or(QJsonObject());

  for (const auto &preference : item->preferences()) {
    QJsonValue defaultValue = preference.defaultValue();
    if (!values.contains(preference.name()) && !defaultValue.isNull()) {
      values[preference.name()] = defaultValue;
    }
  }

  return values;
}

std::vector<Preference> RootItemManager::getMergedItemPreferences(const EntrypointId &id) const {
  auto provider = findProviderById(id.provider.c_str());
  auto item = findItemById(id);

  if (!provider || !item) return {};

  return std::views::concat(provider->preferences(), item->preferences()) | std::ranges::to<std::vector>();
}

QJsonObject RootItemManager::getPreferenceValues(const EntrypointId &id) const {
  /*
auto query = m_db.createQuery();
auto item = findItemById(id);

if (!item) {
qWarning() << "No item with id" << id;
return {};
}

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
*/

  return QJsonObject();
}

RootItemMetadata RootItemManager::itemMetadata(const EntrypointId &id) const {
  if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }

  return {};
}

int RootItemManager::maxFallbackPosition() {
  int max = -1;

  for (const auto &[k, v] : m_metadata) {
    // if (v.fallbackPosition > max) max = v.fallbackPosition;
  }

  return max;
}

bool RootItemManager::isFallback(const EntrypointId &id) const {
  return false;
  // return itemMetadata(id).fallbackPosition != -1;
}

bool RootItemManager::setItemAsFavorite(const EntrypointId &itemId, bool value) {
  /*
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

// m_metadata[itemId].favorite = value;
emit itemFavoriteChanged(itemId, value);
*/

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

std::vector<RootItemManager::SearchableRootItem> RootItemManager::queryFavorites(std::optional<int> limit) {
  /*
auto isFavorite = [](auto &&item) { return item.meta->favorite; };
auto favorites = m_items | std::views::filter(isFavorite) | std::ranges::to<std::vector>();
std::ranges::sort(favorites, [this](const SearchableRootItem &a, const SearchableRootItem &b) {
return a.fuzzyScore() > b.fuzzyScore();
});

if (limit && favorites.size() > limit) { favorites.resize(limit.value()); }

return favorites;
*/
  return {};
}

std::vector<RootItemManager::SearchableRootItem> RootItemManager::querySuggestions(int limit) {
  auto isSuggestable = [](auto &&item) {
    // return item.meta->isEnabled && item.meta->visitCount > 0 && !item.meta->favorite;
    return item.meta->isEnabled && item.meta->visitCount > 0;
  };
  auto suggestions = m_items | std::views::filter(isSuggestable) | std::ranges::to<std::vector>();

  std::ranges::sort(suggestions, [this](const auto &a, const auto &b) {
    auto ascore = computeScore(*a.meta, a.item->baseScoreWeight());
    auto bscore = computeScore(*b.meta, b.item->baseScoreWeight());
    return ascore > bscore;
  });

  if (suggestions.size() > limit) { suggestions.resize(limit); }

  return suggestions;
}

bool RootItemManager::resetRanking(const EntrypointId &id) {
  /*
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
*/

  return true;
}

bool RootItemManager::registerVisit(const EntrypointId &id) {
  /*
qint64 epoch = QDateTime::currentSecsSinceEpoch();
QSqlQuery query = m_db.createQuery();

query.prepare(R"(
          UPDATE root_provider_item
          SET
                  visit_count = visit_count + 1,
                  rank_visit_count = rank_visit_count + 1,
                  last_visited_at = :epoch,
                  rank_last_visited_at = :epoch
          WHERE id = :id
          RETURNING rank_visit_count, rank_last_visited_at, visit_count, last_visited_at
  )");
query.bindValue(":id", id);
query.bindValue(":epoch", epoch);

if (!query.exec() || !query.next()) {
qDebug() << "Failed to update item" << query.lastError();
return false;
}

RootItemMetadata &meta = m_metadata[id];

meta.visitCount = query.value(0).toInt();
meta.lastVisitedAt = std::chrono::system_clock::from_time_t(epoch);
*/

  return false;
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

bool RootItemManager::disableItem(const EntrypointId &id) { return setItemEnabled(id, false); }

bool RootItemManager::enableItem(const EntrypointId &id) { return setItemEnabled(id, true); }

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

RootProvider *RootItemManager::provider(std::string_view id) const {
  auto it = std::ranges::find_if(m_providers, [&id](const auto &p) { return id == p->uniqueId(); });

  if (it != m_providers.end()) return it->get();

  return nullptr;
}
