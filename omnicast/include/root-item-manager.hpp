#pragma once
#include "action-panel/action-panel.hpp"
#include "argument.hpp"
#include "libtrie/trie.hpp"
#include "omni-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "timer.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/default-list-item-widget.hpp"
#include <algorithm>
#include <qdnslookup.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qhash.h>
#include <qtmetamacros.h>
#include <ranges>

struct RootItemPrefixSearchOptions {
  bool includeDisabled = false;
};

class RootItem {
public:
  virtual ~RootItem() = default;

  virtual QString providerId() const = 0;

  virtual QString uniqueId() const = 0;

  /**
   * The name of the item as it will be shown in the root menu.
   * This name is indexed by splitting it in multiple tokens at
   * word boundaries (assuming latin text).
   *
   * Note that camel case names are indexed as multiple tokens and not a single word.
   */
  virtual QString displayName() const = 0;

  virtual OmniIconUrl iconUrl() const = 0;

  /**
   * Whether the item can be selected as a fallback command or not
   */
  virtual bool isSuitableForFallback() const { return false; }

  /**
   * Whether this item should be marked as a fallback command
   * the first time it is ever made available.
   * Only affects the first time the command is loaded.
   */
  virtual bool isDefaultFallback() const { return false; }

  /**
   * What type of item this is. For instance an application will return
   * "Application". This is used in the settings view.
   */
  virtual QString typeDisplayName() const = 0;

  /**
   * An optional list of arguments to be filled in before launching the command.
   * For each argument, a small input field appears next to the search query.
   * Arguments can either be marked as required or optional.
   * The primary action defined for the item will only activate if all the required
   * arguments have been provided.
   */
  virtual ArgumentList arguments() const { return {}; };

  /**
   * An optional list of preferences that can be set in the settings to
   * customize the behaviour of this item.
   */
  virtual PreferenceList preferences() const { return {}; }

  virtual double baseScoreWeight() const { return 1; }

  /**
   * An optional subtitle shown to the left of the `displayName`.
   * Indexed the same as the `displayName`.
   */
  virtual QString subtitle() const { return {}; }

  /**
   * A list of accessories that are shown to the right of
   * the list item.
   */
  virtual AccessoryList accessories() const { return {}; }

  /**
   * List of item-specific actions to display in the action pannel
   * when selected. The first action returned will become the default
   * action.
   */
  virtual QList<AbstractAction *> actions() const { return {}; }

  virtual ActionPanelView *actionPanel() const { return nullptr; }

  /**
   * Action panel shown when this item is used as a fallback command.
   */
  virtual ActionPanelView *fallbackActionPanel() const { return nullptr; }

  /**
   * Alternative list of actions to display in fallback mode.
   * Calls actions() as its default implementation.
   */
  virtual QList<AbstractAction *> fallbackActions() const { return actions(); }

  /**
   * Additional strings that will be indexed and prefix searchable.
   */
  virtual std::vector<QString> keywords() const { return {}; }
};

class RootProvider : public QObject {
  Q_OBJECT

public:
  enum Type {
    ExtensionProvider, // a collection of commands
    GroupProvider,     // a collection of other things
  };

  RootProvider() {}
  virtual ~RootProvider() = default;

  virtual QString uniqueId() const = 0;
  virtual QString displayName() const = 0;
  virtual OmniIconUrl icon() const = 0;
  virtual Type type() const = 0;
  QString typeAsString() {
    switch (type()) {
    case ExtensionProvider:
      return "Extension";
    case GroupProvider:
      return "Group";
    }
  }

  virtual std::vector<std::shared_ptr<RootItem>> loadItems() const = 0;
  virtual PreferenceList preferences() const { return {}; }

signals:
  void itemsChanged() const;
  void itemRemoved(const QString &id) const;
};

class RootItemManager : public QObject {
private:
  Q_OBJECT

  struct RootItemHash {
    size_t operator()(const std::shared_ptr<RootItem> &item) const { return qHash(item->uniqueId()); }
  };

  struct RootItemMetadata {
    int visitCount = 0;
    bool isEnabled = true;
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastVisitedAt;
    // Alias can be made of multiple words, in which case each word is indexed separately
    QString alias;
    bool isFallback = false;
    int fallbackPosition = -1;
    QString providerId;
  };

  struct RootProviderMetadata {};

  Trie<std::shared_ptr<RootItem>, RootItemHash> m_trie;
  std::vector<std::shared_ptr<RootItem>> m_items;
  std::unordered_map<QString, RootItemMetadata> m_metadata;
  std::vector<std::unique_ptr<RootProvider>> m_providers;
  OmniDatabase &m_db;

  RootItemMetadata loadMetadata(const QString &id) {
    RootItemMetadata item;
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		SELECT
			enabled, fallback, fallback_position, alias, visit_count, last_visited_at, provider_id
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

    return item;
  }

  void createTables() {
    QSqlQuery query = m_db.createQuery();

    query.exec(R"(
		CREATE TABLE IF NOT EXISTS root_provider (
			id TEXT PRIMARY KEY,
			preference_values JSON DEFAULT '{}',
			enabled INT DEFAULT 1
		);
	)");

    if (!query.exec(R"(
		CREATE TABLE IF NOT EXISTS root_provider_item (
			id TEXT PRIMARY KEY,
			provider_id TEXT,
			preference_values JSON DEFAULT '{}',
			enabled INT DEFAULT 1,
			fallback INT DEFAULT 0,
			fallback_position INT DEFAULT -1,
			alias TEXT DEFAULT '',
			visit_count INT DEFAULT 0,
			last_visited_at INT,
			FOREIGN KEY(provider_id) 
			REFERENCES root_provider(id)
			ON DELETE CASCADE
		);
	)")) {
      qDebug() << "Failed to create command table" << query.lastError();
    }
  }

  bool upsertProvider(const RootProvider &provider) {
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
      qCritical() << "Failed to upsert provider with id" << provider.uniqueId() << query.lastError();
      return false;
    }

    return true;
  }

  bool upsertItem(const QString &providerId, const RootItem &item) {
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		INSERT INTO 
			root_provider_item (id, provider_id, enabled, fallback) 
		VALUES (:id, :provider_id, :enabled, :fallback) 
		ON CONFLICT(id) DO NOTHING
	)");
    query.bindValue(":id", item.uniqueId());
    query.bindValue(":provider_id", providerId);
    query.bindValue(":enabled", 1);
    query.bindValue(":fallback", item.isDefaultFallback());

    if (!query.exec()) {
      qCritical() << "Failed to upsert provider with id" << item.uniqueId() << query.lastError();
      return false;
    }

    m_metadata[item.uniqueId()] = loadMetadata(item.uniqueId());

    return true;
  }

  void indexItem(const std::shared_ptr<RootItem> &item) {
    auto metadata = itemMetadata(item->uniqueId());
    std::string name = item->displayName().toStdString();
    std::string subtitle = item->subtitle().toStdString();

    m_trie.index(name, item);
    m_trie.indexLatinText(name, item);
    m_trie.indexLatinText(subtitle, item);
    m_trie.indexLatinText(metadata.alias.toStdString(), item);

    for (const auto &keyword : item->keywords()) {
      m_trie.index(keyword.toStdString(), item);
    }
  }

  void rebuildTrie() {
    Timer timer;
    m_trie.clear();
    std::ranges::for_each(m_items, std::bind_front(&RootItemManager::indexItem, this));
    timer.time("trie rebuild");
  }

  void reloadProviders() {
    qDebug() << "reloaded providers!";
    m_items = m_providers | std::views::transform([](const auto &p) { return p->loadItems(); }) |
              std::views::join | std::ranges::to<std::vector>();
    rebuildTrie();
    emit itemsChanged();
  }

  bool setProviderPreferenceValues(const QString &id, const QJsonObject &preferences) {
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

    return true;
  }

  bool setItemPreferenceValues(const QString &id, const QJsonObject &preferences) {
    auto query = m_db.createQuery();
    QJsonDocument json;

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

    return true;
  }

  RootItem *findItemById(const QString &id) {
    auto it = std::ranges::find_if(m_items, [&](auto &&v) { return v->uniqueId() == id; });

    if (it != m_items.end()) return it->get();

    return nullptr;
  }

  RootProvider *findProviderById(const QString &id) {
    auto it = std::ranges::find_if(m_providers, [&](auto &&provider) { return provider->uniqueId() == id; });

    if (it == m_providers.end()) return nullptr;

    return it->get();
  }

public:
  RootItemManager(OmniDatabase &db) : m_db(db) { createTables(); }

  bool setItemEnabled(const QString &id, bool value) {
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

  void setPreferenceValues(const QString &id, const QJsonObject &preferences) {
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

    for (const auto &preference : item->preferences()) {
      auto &prefId = preference->name();
      bool isRepositoryPreference = false;

      if (provider) {
        for (const auto &repoPref : provider->preferences()) {
          if (repoPref->name() == prefId) {
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

  bool setAlias(const QString &id, const QString &alias) {
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

    m_trie.removeLatinTextItem(metadata.alias.toStdString(), *it);
    m_trie.removeLatinTextItem(metadata.alias.toStdString(), *it);
    m_trie.indexLatinText(alias.toStdString(), *it);
    metadata.alias = alias;
    m_metadata[id] = metadata;

    qDebug() << "Set alias";

    return true;
  }

  bool clearAlias(const QString &id) { return setAlias(id, ""); }

  QJsonObject getPreferenceValues(const QString &id) const {
    auto query = m_db.createQuery();
    auto it = std::ranges::find_if(m_items, [&](auto &&item) { return item->uniqueId() == id; });

    if (it == m_items.end()) {
      qCritical() << "No item with id" << id;
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
      qDebug() << "No results";
      return {};
    }
    auto rawJson = query.value(0).toString();

    qDebug() << "raw preferences json" << rawJson;

    auto json = QJsonDocument::fromJson(rawJson.toUtf8());
    auto preferenceValues = json.object();

    for (auto pref : item->preferences()) {
      auto dflt = pref->defaultValueAsJson();

      if (!preferenceValues.contains(pref->name()) && !dflt.isNull()) {
        preferenceValues[pref->name()] = dflt;
      }
    }

    return preferenceValues;
  }

  RootItemMetadata itemMetadata(const QString &id) const {
    if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }

    return {};
  }

  int maxFallbackPosition() {
    int max = -1;

    for (const auto &[k, v] : m_metadata) {
      if (v.fallbackPosition > max) max = v.fallbackPosition;
    }

    return max;
  }

  bool isFallback(const QString &id) { return itemMetadata(id).isFallback; }

  bool disableFallback(const QString &id) {
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

  bool setFallback(const QString &id, int position = 0) {
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

  bool registerVisit(const QString &id) {
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		UPDATE root_provider_item 
		SET 
			visit_count = visit_count + 1, 
			last_visited_at = unixepoch() 
		WHERE id = :id
		RETURNING visit_count, last_visited_at
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

  QString getItemProviderId(const QString &id) {
    auto metadata = itemMetadata(id);

    return metadata.providerId;
  }

  bool disableItem(const QString &id) { return setItemEnabled(id, false); }
  bool enableItem(const QString &id) { return setItemEnabled(id, true); }

  std::vector<RootProvider *> providers() const {
    return m_providers | std::views::transform([](const auto &p) { return p.get(); }) |
           std::ranges::to<std::vector>();
  }

  void addProvider(std::unique_ptr<RootProvider> provider) {
    auto items = provider->loadItems();

    if (!upsertProvider(*provider.get())) return;

    std::ranges::for_each(items, [&](const auto &item) { upsertItem(provider->uniqueId(), *item.get()); });

    m_items.insert(m_items.end(), items.begin(), items.end());
    connect(provider.get(), &RootProvider::itemsChanged, this, &RootItemManager::reloadProviders);
    m_providers.emplace_back(std::move(provider));
    rebuildTrie();
    emit itemsChanged();
  }

  RootProvider *provider(const QString &id) const {
    auto it = std::ranges::find_if(m_providers, [&id](const auto &p) { return id == p->uniqueId(); });

    if (it != m_providers.end()) return it->get();

    return nullptr;
  }

  std::vector<std::shared_ptr<RootItem>> allItems() const { return m_items; }

  std::vector<std::shared_ptr<RootItem>> prefixSearch(const QString &query,
                                                      const RootItemPrefixSearchOptions &opts = {}) {
    std::vector<std::shared_ptr<RootItem>> items;

    for (const auto &item : m_trie.prefixSearch(query.toStdString())) {
      if (!opts.includeDisabled) {
        auto meta = itemMetadata(item->uniqueId());

        if (meta.isEnabled) { items.emplace_back(item); }
      } else {
        items.emplace_back(item);
      }
    }

    std::ranges::sort(items, [this](const auto &a, const auto &b) {
      auto ameta = itemMetadata(a->uniqueId());
      auto bmeta = itemMetadata(b->uniqueId());

      return std::max(ameta.visitCount, 1) * a->baseScoreWeight() >
             std::max(bmeta.visitCount, 1) * b->baseScoreWeight();
    });

    return items;
  }

signals:
  void itemsChanged() const;
  void fallbackEnabled(const QString &id) const;
  void fallbackDisabled(const QString &id) const;
};
