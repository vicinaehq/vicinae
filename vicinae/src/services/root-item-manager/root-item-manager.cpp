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
#include "vicinae.hpp"
#include <ranges>

RootItemManager::RootItemManager(config::Manager &cfg)
    : m_cfg(cfg), m_visitTracker(Omnicast::dataDir() / "visits.json") {
  connect(&cfg, &config::Manager::configChanged, this, [this](const config::ConfigValue &next) {
    mergeConfigWithMetadata(next);
    emit itemsChanged();
  });
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::fallbackItems() const {
  return getFromSerializedEntrypointIds(m_cfg.value().fallbacks);
}

bool RootItemManager::moveFallbackDown(const EntrypointId &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    if (auto fbs = v.fallbacks) {
      auto it = std::ranges::find(fbs.value(), std::string{id});
      if (it != fbs->end()) { std::iter_swap(it, it + 1); }
    }
  });
  emit fallbackOrderChanged(id);
  return true;
}

bool RootItemManager::moveFallbackUp(const EntrypointId &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    if (auto fbs = v.fallbacks) {
      auto it = std::ranges::find(fbs.value(), std::string{id});
      if (it != fbs->end() && it != fbs->begin()) { std::iter_swap(it, it - 1); }
    }
  });
  emit fallbackOrderChanged(id);
  return true;
}

bool RootItemManager::enableFallback(const EntrypointId &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    auto fbs = v.fallbacks.value_or({});
    fbs.insert(fbs.begin(), id);
    v.fallbacks = fbs;
  });
  emit fallbackEnabled(id);
  return true;
}

bool RootItemManager::disableFallback(const EntrypointId &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    if (auto fbs = v.fallbacks) { fbs->erase(std::ranges::find(fbs.value(), std::string{id})); }
  });
  emit fallbackDisabled(id);
  return true;
}

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

  m_items.clear();

  auto &cfg = m_cfg.value();

  for (const auto &provider : m_providers) {
    auto items = provider->loadItems();
    const config::ProviderData *providerConfig = nullptr;

    if (auto it = cfg.providers.find(provider->uniqueId().toStdString()); it != cfg.providers.end()) {
      providerConfig = &it->second;
    }

    for (const auto &item : items) {
      const config::ProviderItemData *itemConfig = nullptr;

      if (providerConfig) {
        if (auto it = providerConfig->entrypoints.find(item->uniqueId().entrypoint);
            it != providerConfig->entrypoints.end()) {
          itemConfig = &it->second;
        }
      }

      SearchableRootItem sitem;
      auto id = item->uniqueId();

      sitem.item = item;
      sitem.title = item->displayName().toStdString();
      sitem.subtitle = item->subtitle().toStdString();
      sitem.keywords = item->keywords() | std::views::transform([](auto &&s) { return s.toStdString(); }) |
                       std::ranges::to<std::vector>();

      sitem.meta = &m_metadata[id];
      sitem.meta->item = item;

      auto visitInfo = m_visitTracker.getVisit(id);

      sitem.meta->visitCount = visitInfo.visitCount;
      m_items.emplace_back(sitem);
    }
  }

  mergeConfigWithMetadata(m_cfg.value());
  m_scoredItems.reserve(m_items.size());
  isReloading = false;
  emit itemsChanged();
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
  std::string pattern = query.toStdString();
  std::string_view patternView = pattern;

  m_scoredItems.clear();

  for (auto &item : m_items) {
    if (!item.meta->enabled && !opts.includeDisabled) continue;
    if (item.meta->favorite && !opts.includeFavorites) continue;
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

  return m_scoredItems;
}

bool RootItemManager::setItemEnabled(const EntrypointId &id, bool value) {
  auto merged = m_cfg.mergeEntrypointWithUser(id, {.enabled = value});
  return true;
}

bool RootItemManager::setProviderPreferenceValues(const QString &id, const QJsonObject &preferences) {
  auto provider = findProviderById(id);

  if (!provider) return false;

  m_cfg.mergeProviderWithUser(id.toStdString(), {.preferences = transformPreferenceValues(preferences)});
  provider->preferencesChanged(preferences);

  return true;
}

QJsonObject RootItemManager::transformPreferenceValues(const glz::generic::object_t &preferences) {
  QJsonObject obj;

  for (const auto &[key, v] : preferences) {
    QString k = key.c_str();
    if (v.is_boolean()) obj[k] = v.get_boolean();
    if (v.is_string()) obj[k] = v.get_string().c_str();
    if (v.is_number()) obj[k] = v.get_number();
    if (v.is_null()) obj[k] = QJsonValue::Null;
  }

  return obj;
}

glz::generic::object_t RootItemManager::transformPreferenceValues(const QJsonObject &preferences) {
  glz::generic::object_t obj;

  for (const auto &key : preferences.keys()) {
    std::string k = key.toStdString();
    QJsonValue v = preferences.value(key);

    if (v.isBool()) obj[k] = v.toBool();
    if (v.isString()) obj[k] = v.toString().toStdString();
    if (v.isDouble()) obj[k] = v.toDouble();
    if (v.isNull()) obj[k] = glz::generic::null_t{};
  }

  return obj;
}

bool RootItemManager::setItemPreferenceValues(const EntrypointId &id, const QJsonObject &preferences) {
  RootItem *item = findItemById(id);

  if (!item) return false;

  m_cfg.mergeEntrypointWithUser(id, {.preferences = transformPreferenceValues(preferences)});
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

  // clang-format off
  m_cfg.mergeWithUser({
		  .providers = config::ProviderMap{
		  	{id.provider, {
				.preferences = transformPreferenceValues(providerPreferenceValues),
				.entrypoints = {
					{id.entrypoint, {.preferences = transformPreferenceValues(entrypointPreferenceValues)}}
				}
			}
		  }
		  }
  });
  // clang-format on

  qWarning() << "setPreferences not yet implemented";
}

bool RootItemManager::setAlias(const EntrypointId &id, std::string_view alias) {
  auto &meta = m_metadata[id];

  m_metadata[id].alias = alias;
  m_cfg.mergeEntrypointWithUser(id, {.alias = std::string{alias}});

  return true;
}

QJsonObject RootItemManager::getProviderPreferenceValues(const QString &id) const {
  auto provider = findProviderById(id);
  auto json = transformPreferenceValues(m_cfg.user().providerPreferences(id.toStdString()).value_or({}));

  for (const auto &pref : provider->preferences()) {
    auto dflt = pref.defaultValue();

    if (!json.contains(pref.name()) && !dflt.isNull()) { json[pref.name()] = dflt; }
  }

  return json;
}

bool RootItemManager::pruneProvider(const QString &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    if (v.providers) { v.providers->erase(id.toStdString()); }
  });

  return true;
}

QJsonObject RootItemManager::getItemPreferenceValues(const EntrypointId &id) const {
  auto item = findItemById(id);

  if (!item) { return {}; }

  // QJsonObject values = itemMetadata(id).preferences.value_or(QJsonObject());
  //  TODO: implement
  QJsonObject values;

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
  auto preferenceValues = transformPreferenceValues(m_cfg.user().mergedPreferences(id));

  for (auto pref : getMergedItemPreferences(id)) {
    auto dflt = pref.defaultValue();
    auto value = preferenceValues.value(pref.name());
    auto hasValue = !value.isUndefined() && !value.isNull();

    if (!hasValue && !dflt.isUndefined()) { preferenceValues[pref.name()] = dflt; }
  }

  return preferenceValues;
}

RootItemMetadata RootItemManager::itemMetadata(const EntrypointId &id) const {
  if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }

  return {};
}

int RootItemManager::maxFallbackPosition() {
  // TODO: implement
  return 0;
}

bool RootItemManager::isFallback(const EntrypointId &id) const {
  return std::ranges::contains(m_cfg.value().fallbacks, std::string{id});
}

bool RootItemManager::setItemAsFavorite(const EntrypointId &itemId, bool value) {
  auto favorites = m_cfg.value().favorites;
  std::string id{itemId};

  if (std::ranges::contains(favorites, id)) { return true; }

  favorites.insert(favorites.begin(), id);
  m_cfg.mergeWithUser({.favorites = favorites});
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

std::vector<std::shared_ptr<RootItem>> RootItemManager::queryFavorites(std::optional<int> limit) {
  return getFromSerializedEntrypointIds(m_cfg.value().favorites);
}

std::vector<RootItemManager::SearchableRootItem> RootItemManager::querySuggestions(int limit) {
  auto isSuggestable = [](auto &&item) {
    return item.meta->enabled && item.meta->visitCount > 0 && !item.meta->favorite;
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
  m_metadata[id].visitCount = 0;
  m_visitTracker.forget(id);
  return true;
}

bool RootItemManager::registerVisit(const EntrypointId &id) {
  ++m_metadata[id].visitCount;
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
  auto favoriteSet = cfg.favorites | std::ranges::to<std::unordered_set>();
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
    meta.favorite = favoriteSet.contains(entrypointId);

    if (itemConfig) {
      if (auto enabled = itemConfig->enabled) { meta.enabled = enabled.value(); }
      if (auto alias = itemConfig->alias) { meta.alias = alias.value(); }
    }

    if (providerConfig) {
      if (auto enabled = providerConfig->enabled) { meta.enabled = enabled.value(); }
    }
  }
}
