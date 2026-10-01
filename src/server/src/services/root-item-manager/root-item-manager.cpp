#include <iterator>
#include <qjsonvalue.h>
#include <ranges>
#include <algorithm>
#include <unordered_map>
#include <qlogging.h>
#include "root-item-manager.hpp"
#include <glaze/json/patch.hpp>
#include "glaze-qt.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "fuzzy/fzf.hpp"
#include "config/config.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "utils.hpp"
#include "vicinae.hpp"

RootItemManager::RootItemManager(config::Manager &cfg, LocalStorageService &storage)
    : m_cfg(cfg), m_storage(storage), m_visitTracker(Omnicast::dataDir() / "metadata.json"),
      m_searchHistory(Omnicast::dataDir() / "search-history.json") {
  connect(&cfg, &config::Manager::configChanged, this, [this](const config::ConfigValue &next) {
    mergeConfigWithMetadata(next);
    syncPreferences();
    emit metadataChanged();
  });
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::fallbackItems() const {
  return getFromSerializedEntrypointIds(m_cfg.value().fallbacks);
}

std::size_t RootItemManager::favoriteCount() const { return m_cfg.value().favorites.size(); }

bool RootItemManager::moveFallbackDown(const EntrypointId &id) {
  auto fbs = m_cfg.value().fallbacks;
  auto it = std::ranges::find(fbs, std::string{id});

  if (it != fbs.end()) { std::iter_swap(it, it + 1); }
  m_cfg.mergeWithUser({.fallbacks = fbs});
  emit fallbackOrderChanged(id);

  return true;
}

bool RootItemManager::moveFallbackUp(const EntrypointId &id) {
  auto fbs = m_cfg.value().fallbacks;
  auto it = std::ranges::find(fbs, std::string{id});

  if (it != fbs.end() && it != fbs.begin()) { std::iter_swap(it, it - 1); }

  m_cfg.mergeWithUser({.fallbacks = fbs});
  emit fallbackOrderChanged(id);

  return true;
}

bool RootItemManager::enableFallback(const EntrypointId &id) {
  auto fbs = m_cfg.value().fallbacks;
  std::string const sid = id;

  if (std::ranges::contains(fbs, sid)) return false;

  fbs.insert(fbs.begin(), sid);
  m_cfg.mergeWithUser({.fallbacks = fbs});
  emit fallbackEnabled(id);
  emit metadataChanged();

  return true;
}

bool RootItemManager::disableFallback(const EntrypointId &id) {
  auto fbs = m_cfg.value().fallbacks;
  auto it = std::ranges::find(fbs, std::string{id});

  if (it == fbs.end()) return false;

  fbs.erase(it);
  m_cfg.mergeWithUser({.fallbacks = fbs});
  emit fallbackDisabled(id);
  emit metadataChanged();

  return true;
}

RootItem *RootItemManager::findItemById(const EntrypointId &id) const {
  if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second.item.get(); }

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
  m_items.clear();
  m_metadata.clear();

  for (const auto &provider : m_providers) {
    auto items = provider->loadItems();

    for (const auto &item : items) {
      // we build data ready to be searched on once during indexing, so that
      // subsequent searches are not affected by useless conversions/copies.
      SearchableRootItem sitem;
      auto id = item->uniqueId();

      sitem.item = item;
      sitem.title = item->title().toStdString();
      if (auto unlocalized = item->unlocalizedTitle(); unlocalized && *unlocalized != item->title()) {
        sitem.unlocalizedTitle = unlocalized->toStdString();
      }
      sitem.subtitle = item->subtitle().toStdString();
      sitem.keywords = Utils::toStdStringVec(item->keywords());
      sitem.meta = &m_metadata[id];
      sitem.meta->item = item;

      auto visitInfo = m_visitTracker.getVisit(id);

      sitem.meta->visitCount = visitInfo.visitCount;
      sitem.meta->lastVisitedAt = visitInfo.lastVisitedAt;
      m_items.emplace_back(sitem);
    }
  }

  mergeConfigWithMetadata(m_cfg.value());
  syncPreferences();
  isReloading = false;
  emit itemsChanged();
}

double RootItemManager::SearchableRootItem::frecency() const {
  return fuzzy::frecency(meta->visitCount, meta->lastVisitedAt, QDateTime::currentSecsSinceEpoch());
}

double RootItemManager::SearchableRootItem::fuzzyScore(const fuzzy::Query &query) const {
  if (query.empty()) return 100.0 - fuzzy::FRECENCY_WEIGHT + fuzzy::FRECENCY_WEIGHT * frecency();

  using WS = fzf::WeightedString;
  std::string alias = meta->alias.value_or("");
  std::initializer_list<WS> ss = {{title, 1.0f}, {unlocalizedTitle, 1.0f}, {subtitle, 0.5f}, {alias, 1.0f}};
  auto kws = keywords | std::views::transform([](auto &&kw) { return WS{kw, 0.6f}; });
  auto const score = fzf::threadLocalMatcher().score_query(ss, kws, query);

  if (score.quality < fuzzy::MIN_QUALITY) return 0;

  return score.score + fuzzy::FRECENCY_WEIGHT * frecency();
}

std::vector<RootItemManager::ScoredItem> RootItemManager::search(const QString &query,
                                                                 const RootItemPrefixSearchOptions &opts) {
  std::vector<ScoredItem> items;
  search(query, items, opts);
  return items;
}

void RootItemManager::search(const QString &query, std::vector<ScoredItem> &results,
                             const RootItemPrefixSearchOptions &opts) {
  std::string pattern = query.toStdString();
  fuzzy::Query const fuzzyQuery{pattern};

  results.clear();
  results.reserve(m_items.size());

  for (auto &item : m_items) {
    if (!item.meta->enabled && !opts.includeDisabled) continue;
    if (opts.providerId && opts.providerId != item.meta->providerId) continue;
    if (item.meta->favoriteIdx.has_value() && !opts.includeFavorites) continue;
    double const fuzzyScore = item.fuzzyScore(fuzzyQuery);

    if (!fuzzyScore) { continue; }

    results.emplace_back(ScoredItem{.meta = item.meta, .score = fuzzyScore, .item = item.item});
  }

  // we need stable sort to avoid flickering when updating quickly
  std::ranges::stable_sort(results, [&](const auto &a, const auto &b) {
    if (opts.prioritizeAliased) {
      bool const aa = !a.meta->alias.value_or("").empty() && a.meta->alias->starts_with(pattern);
      bool const ab = !b.meta->alias.value_or("").empty() && b.meta->alias->starts_with(pattern);
      // always prioritize matching aliases over score
      if (aa != ab) { return aa > ab; }
      if (aa && ab && a.meta->alias->size() != b.meta->alias->size()) {
        return a.meta->alias->size() < b.meta->alias->size();
      }
    }

    return a.score > b.score;
  });
}

std::vector<RootItemManager::ProviderSearchGroup>
RootItemManager::searchGroupedByProvider(const QString &query, const RootItemPrefixSearchOptions &opts) {
  fuzzy::Query const fuzzyQuery{query.toStdString()};

  std::unordered_map<std::string, RootProvider *> providerById;
  std::unordered_map<std::string, double> providerNameScore;
  for (auto *provider : providers()) {
    if (provider->isTransient()) continue;
    auto id = provider->uniqueId().toStdString();
    providerById.emplace(id, provider);
    auto const m = fuzzy::scoreWeighted({{provider->displayName().toStdString(), 1.0}}, fuzzyQuery);
    if (m.accepted()) providerNameScore.emplace(id, static_cast<double>(m.score));
  }

  struct ScoredEntry {
    double score = 0;
    ItemPtr item;
    bool enabled = true;
  };
  struct Bucket {
    double best = 0;
    std::vector<ScoredEntry> entries;
  };
  std::unordered_map<std::string, Bucket> buckets;

  for (auto &item : m_items) {
    if (!item.meta->enabled && !opts.includeDisabled) continue;
    if (item.meta->favoriteIdx.has_value() && !opts.includeFavorites) continue;

    const auto &providerId = item.meta->providerId;
    if (!providerById.contains(providerId)) continue;

    double const titleScore = item.fuzzyScore(fuzzyQuery);
    auto nameIt = providerNameScore.find(providerId);
    bool const providerMatched = nameIt != providerNameScore.end();
    if (titleScore <= 0 && !providerMatched) continue;

    auto &bucket = buckets[providerId];
    bucket.entries.push_back({titleScore, item.item, item.meta->enabled});
    bucket.best = std::max(bucket.best, std::max<double>(titleScore, providerMatched ? nameIt->second : 0.0));
  }

  std::vector<ProviderSearchGroup> groups;
  groups.reserve(buckets.size());
  for (auto &[id, bucket] : buckets) {
    std::ranges::stable_sort(bucket.entries, [](const auto &a, const auto &b) { return a.score > b.score; });
    ProviderSearchGroup group{.provider = providerById[id], .score = bucket.best};
    group.items.reserve(bucket.entries.size());
    for (auto &entry : bucket.entries) {
      group.items.push_back({std::move(entry.item), entry.enabled});
    }
    groups.push_back(std::move(group));
  }

  std::ranges::stable_sort(groups, [](const auto &a, const auto &b) { return a.score > b.score; });
  return groups;
}

bool RootItemManager::setItemEnabled(const EntrypointId &id, bool value) {
  m_cfg.mergeEntrypointWithUser(id, {.enabled = value});
  return true;
}

bool RootItemManager::setProviderPreferenceValues(const QString &id, const PreferenceValues &preferences) {
  auto provider = findProviderById(id);

  if (!provider) return false;

  PreferenceValues filtered;

  for (const Preference &pref : provider->preferences()) {
    const auto *value = preferences::find(preferences, pref.name().toStdString());
    if (!value) continue;
    if (pref.isSecret()) {
      setProviderSecretPreference(id, pref.name(), *value);
    } else {
      filtered[pref.name().toStdString()] = *value;
    }
  }

  m_cfg.mergeProviderWithUser(id.toStdString(), {.preferences = std::move(filtered)});
  syncProviderPreferences(*provider);

  return true;
}

bool RootItemManager::setItemPreferenceValues(const EntrypointId &id, const PreferenceValues &preferences) {
  RootItem const *item = findItemById(id);

  if (!item) return false;

  PreferenceValues filtered;

  for (const Preference &pref : item->preferences()) {
    const auto *value = preferences::find(preferences, pref.name().toStdString());
    if (!value) continue;
    if (pref.isSecret()) {
      setEntrypointSecretPreference(id, pref.name(), *value);
    } else {
      filtered[pref.name().toStdString()] = *value;
    }
  }

  m_cfg.mergeEntrypointWithUser(id, {.preferences = std::move(filtered)});
  syncItemPreferences(*item);

  return true;
}

ScopedLocalStorage RootItemManager::getProviderSecretStorage(const QString &id) const {
  return m_storage.scoped(id + ":preferences");
}

void RootItemManager::setPreferenceValues(const EntrypointId &id, const PreferenceValues &preferences) {
  auto item = findItemById(id);
  auto prvd = provider(id.provider);

  if (!item) {
    qWarning() << "setPreferenceValues: no item with id" << std::string{id};
    return;
  }

  PreferenceValues providerValues;
  PreferenceValues entrypointValues;

  for (const auto &pref : prvd->preferences()) {
    const auto *value = preferences::find(preferences, pref.name().toStdString());
    if (!value) continue;
    if (pref.isSecret()) {
      setProviderSecretPreference(id.provider.c_str(), pref.name(), *value);
    } else {
      providerValues[pref.name().toStdString()] = *value;
    }
  }

  for (const auto &pref : item->preferences()) {
    const auto *value = preferences::find(preferences, pref.name().toStdString());
    if (!value) continue;
    if (pref.isSecret()) {
      setEntrypointSecretPreference(id, pref.name(), *value);
    } else {
      entrypointValues[pref.name().toStdString()] = *value;
    }
  }

  // clang-format off
  m_cfg.mergeWithUser({
		  .providers = std::map<std::string, config::Partial<config::ProviderData>>{
		  	{id.provider, config::Partial<config::ProviderData>{
				.preferences = std::move(providerValues),
				.entrypoints = std::map<std::string, config::ProviderItemData>{
					{id.entrypoint, {.preferences = std::move(entrypointValues)}}
				}
			}
		  }
		}
  });
  // clang-format on
  syncProviderPreferences(*prvd);
  syncItemPreferences(*item);
}

bool RootItemManager::setAlias(const EntrypointId &id, std::string_view alias) {
  m_metadata[id].alias = alias;
  m_cfg.mergeEntrypointWithUser(id, {.alias = std::string{alias}});

  return true;
}

bool RootItemManager::setShortcut(const EntrypointId &id, std::string_view shortcut) {
  if (shortcut.empty()) {
    m_metadata[id].shortcut.reset();
  } else {
    m_metadata[id].shortcut = shortcut;
  }
  m_cfg.mergeEntrypointWithUser(id, {.shortcut = std::string{shortcut}});

  return true;
}

PreferenceValues RootItemManager::getProviderPreferenceValues(const QString &id) const {
  auto provider = findProviderById(id);
  auto values = m_cfg.value().providerPreferences(id.toStdString()).value_or(PreferenceValues{});

  for (const Preference &pref : provider->preferences()) {
    const auto key = pref.name().toStdString();
    if (values.contains(key)) continue;
    if (pref.isSecret()) {
      auto secret = getProviderSecretPreference(id, pref.name());
      values[key] = secret.is_null() ? pref.defaultOrNull() : std::move(secret);
    } else {
      values[key] = pref.defaultOrNull();
    }
  }

  return values;
}

bool RootItemManager::pruneProvider(const QString &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    if (v.providers) { v.providers->erase(id.toStdString()); }
  });

  m_storage.clearNamespace(id + ":preferences");
  m_storage.clearNamespace(id + ":data");

  return true;
}

PreferenceValues RootItemManager::getItemPreferenceValues(const EntrypointId &id) const {
  auto item = findItemById(id);

  if (!item) return {};

  auto values = m_cfg.value().preferences(id).value_or(PreferenceValues{});

  for (const auto &pref : item->preferences()) {
    const auto key = pref.name().toStdString();
    if (values.contains(key)) continue;
    if (pref.isSecret()) {
      auto secret = getEntrypointSecretPreference(id, pref.name());
      values[key] = secret.is_null() ? pref.defaultOrNull() : std::move(secret);
    } else {
      values[key] = pref.defaultOrNull();
    }
  }

  return values;
}

std::vector<Preference> RootItemManager::getMergedItemPreferences(const EntrypointId &id) const {
  auto provider = findProviderById(id.provider.c_str());
  auto item = findItemById(id);

  if (!provider || !item) return {};

  auto result = provider->preferences() | std::ranges::to<std::vector>();
  auto itemPrefs = item->preferences();
  result.insert(result.end(), itemPrefs.begin(), itemPrefs.end());
  return result;
}

PreferenceValues RootItemManager::getPreferenceValues(const EntrypointId &id) const {
  auto values = getProviderPreferenceValues(id.provider.c_str());

  for (auto &[key, value] : getItemPreferenceValues(id)) {
    values.insert_or_assign(key, std::move(value));
  }

  return values;
}

RootItemMetadata RootItemManager::itemMetadata(const EntrypointId &id) const {
  if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }
  return {};
}

bool RootItemManager::isFallback(const EntrypointId &id) const {
  return std::ranges::contains(m_cfg.value().fallbacks, std::string{id});
}

bool RootItemManager::setItemAsFavorite(const EntrypointId &itemId, bool value) {
  auto favorites = m_cfg.value().favorites; // we take the merged config to account for default favorites
  std::string const id{itemId};

  if (value) {
    favorites.insert(favorites.begin(), id);
  } else {
    auto it = std::ranges::find(favorites, id);
    if (it == favorites.end()) { return false; }
    favorites.erase(it);
  }

  m_cfg.mergeWithUser({.favorites = favorites});
  emit itemFavoriteChanged(itemId, value);
  emit metadataChanged();

  return true;
}

bool RootItemManager::moveFavoriteDown(const EntrypointId &id) {
  auto favorites = m_cfg.value().favorites;
  auto it = std::ranges::find(favorites, std::string{id});

  if (it == favorites.end() || it + 1 == favorites.end()) return false;

  std::iter_swap(it, it + 1);
  m_cfg.mergeWithUser({.favorites = favorites});
  emit favoriteOrderChanged(id);

  return true;
}

bool RootItemManager::moveFavoriteUp(const EntrypointId &id) {
  auto favorites = m_cfg.value().favorites;
  auto it = std::ranges::find(favorites, std::string{id});

  if (it == favorites.end() || it == favorites.begin()) return false;

  std::iter_swap(it, it - 1);
  m_cfg.mergeWithUser({.favorites = favorites});
  emit favoriteOrderChanged(id);

  return true;
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::queryFavorites(std::optional<int> limit) {
  return getFromSerializedEntrypointIds(m_cfg.value().favorites);
}

bool RootItemManager::resetRanking(const EntrypointId &id) {
  m_metadata[id].visitCount = 0;
  m_metadata[id].lastVisitedAt.reset();
  m_visitTracker.forget(id);
  return true;
}

bool RootItemManager::registerVisit(const EntrypointId &id) {
  ++m_metadata[id].visitCount;
  m_metadata[id].lastVisitedAt = QDateTime::currentSecsSinceEpoch();
  m_visitTracker.registerVisit(id);
  return true;
}

bool RootItemManager::setProviderEnabled(const QString &providerId, bool value) {
  m_cfg.mergeProviderWithUser(providerId.toStdString(), {.enabled = value});
  return true;
}

bool RootItemManager::disableItem(const EntrypointId &id) { return setItemEnabled(id, false); }

bool RootItemManager::enableItem(const EntrypointId &id) { return setItemEnabled(id, true); }

std::vector<RootProvider *> RootItemManager::providers() const {
  std::vector<RootProvider *> providers;

  providers.reserve(m_providers.size());
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
    if (auto p = dynamic_cast<ExtensionRootProvider *>(provider.get())) { providers.emplace_back(p); }
  }

  return providers;
}

void RootItemManager::unloadProvider(const QString &id) {
  auto it = std::ranges::find_if(m_providers, [&](auto &&p) { return p->uniqueId() == id; });

  if (it == m_providers.end()) return;

  m_dispatchedProviderPreferences.erase(id.toStdString());
  m_providers.erase(it);
}

void RootItemManager::loadProvider(std::unique_ptr<RootProvider> provider) {
  auto pred = [&](auto &&p) { return p->uniqueId() == provider->uniqueId(); };
  auto it = std::ranges::find_if(m_providers, pred);

  if (it != m_providers.end()) {
    *it = std::move(provider);
    return;
  }

  auto ptr = provider.get();

  m_providers.emplace_back(std::move(provider));
  ptr->initialized(dispatchProviderPreferences(*ptr));
  connect(ptr, &RootProvider::itemsChanged, this, [this]() { updateIndex(); });
}

RootProvider *RootItemManager::provider(std::string_view id) const {
  auto it = std::ranges::find_if(m_providers, [&id](const auto &p) { return id == p->uniqueId(); });

  if (it != m_providers.end()) return it->get();

  return nullptr;
}

QString RootItemManager::getEntrypointSecretPreferenceKey(const EntrypointId &id, const QString &prefName) {
  return QString("%1.%2").arg(id.entrypoint.c_str()).arg(prefName);
}

glz::generic RootItemManager::getEntrypointSecretPreference(const EntrypointId &id,
                                                            const QString &prefName) const {
  QString const key = getEntrypointSecretPreferenceKey(id, prefName);
  return qJsonValueToGlazeGeneric(getProviderSecretStorage(id.provider.c_str()).getItem(key));
}

void RootItemManager::setEntrypointSecretPreference(const EntrypointId &id, const QString &prefName,
                                                    const glz::generic &value) {
  QString const key = getEntrypointSecretPreferenceKey(id, prefName);
  getProviderSecretStorage(id.provider.c_str()).setItem(key, glazeToQJsonValue(value));
}

glz::generic RootItemManager::getProviderSecretPreference(const QString &providerId,
                                                          const QString &prefName) const {
  return qJsonValueToGlazeGeneric(getProviderSecretStorage(providerId).getItem(prefName));
}

void RootItemManager::setProviderSecretPreference(const QString &id, const QString &prefName,
                                                  const glz::generic &value) {
  getProviderSecretStorage(id).setItem(prefName, glazeToQJsonValue(value));
}

std::vector<std::shared_ptr<RootItem>>
RootItemManager::getFromSerializedEntrypointIds(std::span<const std::string> ids) const {
  std::vector<std::shared_ptr<RootItem>> entrypoints;

  entrypoints.reserve(ids.size());

  for (const auto &id : ids) {
    auto entrypointId = EntrypointId::fromSerialized(id);

    if (auto it = m_metadata.find(entrypointId); it != m_metadata.end()) {
      entrypoints.push_back(it->second.item);
    }
  }

  return entrypoints;
}

void RootItemManager::mergeConfigWithMetadata(const config::ConfigValue &cfg) {
  auto fallbackSet = cfg.fallbacks | std::ranges::to<std::unordered_set>();

  for (const SearchableRootItem &item : m_items) {
    auto entrypointId = item.item->uniqueId();
    const config::ProviderData *providerConfig = nullptr;
    const config::ProviderItemData *itemConfig = nullptr;

    if (auto it = cfg.providers.find(entrypointId.provider); it != cfg.providers.end()) {
      providerConfig = &it->second;
    }

    if (providerConfig) {
      if (auto it = providerConfig->entrypoints.find(entrypointId.entrypoint);
          it != providerConfig->entrypoints.end()) {
        itemConfig = &it->second;
      }
    }

    auto &meta = m_metadata[entrypointId];

    meta.providerId = entrypointId.provider;
    meta.enabled = !item.item->isDefaultDisabled();

    if (auto it = std::ranges::find(cfg.favorites, std::string{entrypointId}); it != cfg.favorites.end()) {
      meta.favoriteIdx = std::distance(cfg.favorites.begin(), it);
    }

    meta.fallback = fallbackSet.contains(entrypointId);

    if (itemConfig) {
      if (auto enabled = itemConfig->enabled) { meta.enabled = enabled.value(); }
      if (auto alias = itemConfig->alias) { meta.alias = alias.value(); }
      if (auto shortcut = itemConfig->shortcut) { meta.shortcut = shortcut.value(); }
    }

    if (providerConfig) {
      if (auto enabled = providerConfig->enabled; enabled.has_value() && !enabled.value()) {
        meta.enabled = false;
      }
    }
  }
}

bool RootItemManager::samePreferences(const PreferenceValues &a, const PreferenceValues &b) {
  if (a.size() != b.size()) return false;

  return std::ranges::all_of(a, [&](const auto &entry) {
    auto it = b.find(entry.first);
    return it != b.end() && glz::equal(entry.second, it->second);
  });
}

PreferenceValues RootItemManager::dispatchProviderPreferences(RootProvider &provider) {
  auto values = getProviderPreferenceValues(provider.uniqueId());

  provider.preferencesChanged(values);
  m_dispatchedProviderPreferences[provider.uniqueId().toStdString()] = values;

  return values;
}

void RootItemManager::syncProviderPreferences(RootProvider &provider) {
  auto values = getProviderPreferenceValues(provider.uniqueId());
  auto [it, inserted] = m_dispatchedProviderPreferences.try_emplace(provider.uniqueId().toStdString());

  if (!inserted && samePreferences(it->second, values)) return;

  provider.preferencesChanged(values);
  it->second = std::move(values);
}

void RootItemManager::syncItemPreferences(const RootItem &item) {
  if (item.preferences().empty()) return;

  auto id = item.uniqueId();
  auto values = getItemPreferenceValues(id);
  auto [it, inserted] = m_dispatchedItemPreferences.try_emplace(id);

  if (!inserted && samePreferences(it->second, values)) return;

  item.preferenceValuesChanged(values);
  it->second = std::move(values);
}

void RootItemManager::syncPreferences() {
  for (const auto &provider : m_providers) {
    syncProviderPreferences(*provider);
  }

  std::erase_if(m_dispatchedItemPreferences,
                [&](const auto &entry) { return !m_metadata.contains(entry.first); });

  for (const SearchableRootItem &item : m_items) {
    syncItemPreferences(*item.item);
  }
}
