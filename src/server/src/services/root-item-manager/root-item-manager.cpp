#include <qjsonvalue.h>
#include <ranges>
#include <algorithm>
#include <qlogging.h>
#include "root-item-manager.hpp"
#include "common.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "lib/fzf.hpp"
#include "config/config.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "vicinae.hpp"

RootItemManager::RootItemManager(config::Manager &cfg, LocalStorageService &storage)
    : m_cfg(cfg), m_storage(storage), m_visitTracker(Omnicast::dataDir() / "metadata.json") {
  connect(&cfg, &config::Manager::configChanged, this, [this](const config::ConfigValue &next) {
    mergeConfigWithMetadata(next);
    qDebug() << "configuration changed";
    emit metadataChanged();
  });
}

std::vector<std::shared_ptr<RootItem>> RootItemManager::fallbackItems() const {
  return getFromSerializedEntrypointIds(m_cfg.value().fallbacks);
}

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
  std::string sid = id;

  if (std::ranges::contains(fbs, sid)) return false;

  fbs.insert(fbs.begin(), sid);
  m_cfg.mergeWithUser({.fallbacks = fbs});
  emit fallbackEnabled(id);
  emit metadataChanged();

  return true;
}

bool RootItemManager::disableFallback(const EntrypointId &id) {
  auto fbs = m_cfg.value().fallbacks;
  std::string sid = id;

  fbs.erase(std::ranges::find(fbs, sid));
  m_cfg.mergeWithUser({.fallbacks = fbs});
  emit fallbackDisabled(id);
  emit metadataChanged();

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
  m_metadata.clear();

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

std::vector<RootItemManager::ScoredItem> RootItemManager::search(const QString &query,
                                                                 const RootItemPrefixSearchOptions &opts) {
  std::vector<ScoredItem> items;
  search(query, items, opts);
  return items;
}

void RootItemManager::search(const QString &query, std::vector<ScoredItem> &results,
                             const RootItemPrefixSearchOptions &opts) {
  std::string pattern = query.toStdString();
  std::string_view patternView = pattern;

  results.clear();
  results.reserve(m_items.size());

  for (auto &item : m_items) {
    if (!item.meta->enabled && !opts.includeDisabled) continue;
    if (opts.providerId && opts.providerId != item.meta->providerId) continue;
    if (item.meta->favorite && !opts.includeFavorites) continue;
    double fuzzyScore = item.fuzzyScore(patternView);

    if (!fuzzyScore) { continue; }

    results.emplace_back(ScoredItem{.meta = item.meta, .score = fuzzyScore, .item = item.item});
  }

  // we need stable sort to avoid flickering when updating quickly
  std::ranges::stable_sort(results, [&](const auto &a, const auto &b) {
    if (opts.prioritizeAliased) {
      bool aa = !a.meta->alias.value_or("").empty() && a.meta->alias->starts_with(pattern);
      bool ab = !b.meta->alias.value_or("").empty() && b.meta->alias->starts_with(pattern);
      // always prioritize matching aliases over score
      if (aa - ab) { return aa > ab; }
    }

    return a.score > b.score;
  });
}

bool RootItemManager::setItemEnabled(const EntrypointId &id, bool value) {
  auto merged = m_cfg.mergeEntrypointWithUser(id, {.enabled = value});
  return true;
}

bool RootItemManager::setProviderPreferenceValues(const QString &id, const QJsonObject &preferences) {
  auto provider = findProviderById(id);

  if (!provider) return false;

  QJsonObject filteredPreferences;
  auto storage = getProviderSecretStorage(id);

  for (const Preference &pref : provider->preferences()) {
    QJsonValue v = preferences.value(pref.name());
    if (!v.isUndefined()) {
      if (pref.isSecret()) {
        setProviderSecretPreference(id, pref.name(), v);
      } else {
        filteredPreferences[pref.name()] = v;
      }
    }
  }

  m_cfg.mergeProviderWithUser(id.toStdString(),
                              {.preferences = transformPreferenceValues(filteredPreferences)});

  return true;
}

QJsonObject RootItemManager::transformPreferenceValues(const glz::generic::object_t &preferences) {
  QJsonObject obj;

  std::function<QJsonValue(const glz::generic &)> transformValue = [&](const glz::generic &v) -> QJsonValue {
    if (v.is_boolean()) return v.get_boolean();
    if (v.is_string()) return v.get_string().c_str();
    if (v.is_number()) return v.get_number();
    if (v.is_null()) return QJsonValue::Null;
    if (v.is_array())
      return v.get_array() | std::views::transform(transformValue) | std::ranges::to<QJsonArray>();
    if (v.is_object()) return transformValue(v.get_object());
    return QJsonValue::Undefined;
  };

  for (const auto &[key, v] : preferences) {
    obj[key.c_str()] = transformValue(v);
  }

  return obj;
}

glz::generic::object_t RootItemManager::transformPreferenceValues(const QJsonObject &preferences) {
  glz::generic::object_t obj;

  std::function<glz::generic(const QJsonValue &)> transformValue = [&](const QJsonValue &v) -> glz::generic {
    if (v.isBool()) return v.toBool();
    if (v.isString()) return v.toString().toStdString();
    if (v.isDouble()) return v.toDouble();
    if (v.isNull()) return glz::generic::null_t{};
    if (v.isArray())
      return v.toArray() | std::views::transform(transformValue) | std::ranges::to<glz::generic::array_t>();
    if (v.isObject()) return transformPreferenceValues(v.toObject());
    return {};
  };

  for (const auto &key : preferences.keys()) {
    obj[key.toStdString()] = transformValue(preferences.value(key));
  }

  return obj;
}

bool RootItemManager::setItemPreferenceValues(const EntrypointId &id, const QJsonObject &preferences) {
  RootItem *item = findItemById(id);

  if (!item) return false;

  QJsonObject itemPreferences;

  for (const Preference &pref : item->preferences()) {
    QJsonValue v = preferences.value(pref.name());

    if (!v.isUndefined()) {
      if (pref.isSecret()) {
        setEntrypointSecretPreference(id, pref.name(), v);
      } else {
        itemPreferences[pref.name()] = v;
      }
    }
  }

  m_cfg.mergeEntrypointWithUser(id, {.preferences = transformPreferenceValues(itemPreferences)});
  item->preferenceValuesChanged(preferences);

  return true;
}

ScopedLocalStorage RootItemManager::getProviderSecretStorage(const QString &id) const {
  return m_storage.scoped(id + ":preferences");
}

void RootItemManager::setPreferenceValues(const EntrypointId &id, const QJsonObject &preferences) {
  auto item = findItemById(id);
  auto prvd = provider(id.provider);

  QJsonObject providerPreferenceValues;
  QJsonObject entrypointPreferenceValues;

  if (!item) {
    qWarning() << "setPreferenceValues: no item with id" << std::string{id};
    return;
  }

  for (const auto &pref : prvd->preferences()) {
    QJsonValue val = preferences.value(pref.name());
    if (!val.isUndefined()) {
      if (pref.isSecret()) {
        setProviderSecretPreference(id.provider.c_str(), pref.name(), val);
      } else {
        providerPreferenceValues[pref.name()] = val;
      }
    }
  }

  for (const auto &pref : item->preferences()) {
    QJsonValue val = preferences.value(pref.name());

    if (!val.isUndefined()) {
      if (pref.isSecret()) {
        setEntrypointSecretPreference(id, pref.name(), val);
      } else {
        entrypointPreferenceValues[pref.name()] = val;
      }
    }
  }

  // clang-format off
  m_cfg.mergeWithUser({
		  .providers = std::map<std::string, config::Partial<config::ProviderData>>{
		  	{id.provider, config::Partial<config::ProviderData>{
				.preferences = transformPreferenceValues(providerPreferenceValues),
				.entrypoints = std::map<std::string, config::ProviderItemData>{
					{id.entrypoint, {.preferences = transformPreferenceValues(entrypointPreferenceValues)}}
				}
			}
		  }
		}
  });
  // clang-format on
}

bool RootItemManager::setAlias(const EntrypointId &id, std::string_view alias) {
  auto &meta = m_metadata[id];

  m_metadata[id].alias = alias;
  m_cfg.mergeEntrypointWithUser(id, {.alias = std::string{alias}});

  return true;
}

QJsonObject RootItemManager::getProviderPreferenceValues(const QString &id) const {
  auto provider = findProviderById(id);
  auto json = transformPreferenceValues(m_cfg.value().providerPreferences(id.toStdString()).value_or({}));

  for (const Preference &pref : provider->preferences()) {
    if (!json.contains(pref.name())) {
      if (pref.isSecret()) {
        QJsonValue value = getProviderSecretPreference(id, pref.name());
        json[pref.name()] = value.isNull() ? pref.defaultValue() : value;
      } else {
        json[pref.name()] = pref.defaultValue();
      }
    }
  }

  return json;
}

bool RootItemManager::pruneProvider(const QString &id) {
  m_cfg.updateUser([&](config::PartialValue &v) {
    if (v.providers) { v.providers->erase(id.toStdString()); }
  });

  m_storage.clearNamespace(id + ":preferences");
  m_storage.clearNamespace(id + ":data");

  return true;
}

QJsonObject RootItemManager::getItemPreferenceValues(const EntrypointId &id) const {
  auto item = findItemById(id);

  if (!item) return {};

  QJsonObject json = transformPreferenceValues(m_cfg.value().preferences(id).value_or({}));

  for (const auto &pref : item->preferences()) {
    if (!json.contains(pref.name())) {
      if (pref.isSecret()) {
        QJsonValue value = getEntrypointSecretPreference(id, pref.name());
        json[pref.name()] = value.isNull() ? pref.defaultValue() : value;
      } else {
        json[pref.name()] = pref.defaultValue();
      }
    }
  }

  return json;
}

std::vector<Preference> RootItemManager::getMergedItemPreferences(const EntrypointId &id) const {
  auto provider = findProviderById(id.provider.c_str());
  auto item = findItemById(id);

  if (!provider || !item) return {};

  return std::views::concat(provider->preferences(), item->preferences()) | std::ranges::to<std::vector>();
}

QJsonObject RootItemManager::getPreferenceValues(const EntrypointId &id) const {
  QJsonObject providerValues = getProviderPreferenceValues(id.provider.c_str());
  QJsonObject itemValues = getItemPreferenceValues(id);

  for (auto it = itemValues.begin(); it != itemValues.end(); ++it) {
    providerValues[it.key()] = it.value();
  }

  return providerValues;
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
  std::string id{itemId};

  if (value) {
    favorites.insert(favorites.begin(), id);
  } else {
    favorites.erase(std::ranges::find(favorites, id));
  }

  m_cfg.mergeWithUser({.favorites = favorites});
  emit itemFavoriteChanged(itemId, value);
  emit metadataChanged();

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

  ptr->preferencesChanged(preferenceValues);
  ptr->initialized(preferenceValues);
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

QJsonValue RootItemManager::getEntrypointSecretPreference(const EntrypointId &id,
                                                          const QString &prefName) const {
  QString key = getEntrypointSecretPreferenceKey(id, prefName);
  return getProviderSecretStorage(id.provider.c_str()).getItem(key);
}

void RootItemManager::setEntrypointSecretPreference(const EntrypointId &id, const QString &prefName,
                                                    const QJsonValue &value) {
  QString key = getEntrypointSecretPreferenceKey(id, prefName);
  getProviderSecretStorage(id.provider.c_str()).setItem(key, value);
}

QJsonValue RootItemManager::getProviderSecretPreference(const QString &providerId,
                                                        const QString &prefName) const {
  return getProviderSecretStorage(providerId).getItem(prefName);
}

void RootItemManager::setProviderSecretPreference(const QString &id, const QString &prefName,
                                                  const QJsonValue &value) {
  getProviderSecretStorage(id).setItem(prefName, value);
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
    meta.fallback = fallbackSet.contains(entrypointId);

    if (itemConfig) {
      item.item->preferenceValuesChanged(getItemPreferenceValues(entrypointId));
      if (auto enabled = itemConfig->enabled) { meta.enabled = enabled.value(); }
      if (auto alias = itemConfig->alias) { meta.alias = alias.value(); }
    }

    if (providerConfig) {
      if (auto enabled = providerConfig->enabled; enabled.has_value() && !enabled.value()) {
        meta.enabled = false;
      }
    }
  }

  // update provider preferences to make sure they are in sync
  for (const auto &provider : m_providers) {
    provider->preferencesChanged(getProviderPreferenceValues(provider->uniqueId()));
  }
}
