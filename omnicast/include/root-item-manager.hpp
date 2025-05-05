#pragma once
#include "argument.hpp"
#include "extend/metadata-model.hpp"
#include "libtrie/trie.hpp"
#include "omni-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "timer.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/default-list-item-widget.hpp"
#include <algorithm>
#include <qdnslookup.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qhash.h>
#include <qtmetamacros.h>
#include <ranges>

class RootItem {
public:
  virtual ~RootItem() = default;

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
  virtual bool isSuitableForFallback() const { return true; }

  /**
   * Whether this item should be marked as a fallback command
   * the first time it is ever made available.
   * Only affects the first time the command is loaded.
   */
  virtual bool isDefaultFallback() const { return false; }

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
  virtual Type type() const = 0;

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
    std::vector<QString> aliases;
    bool isFallback = false;
  };

  struct RootProviderMetadata {};

  Trie<std::shared_ptr<RootItem>, RootItemHash> m_trie;
  std::vector<std::shared_ptr<RootItem>> m_items;
  std::unordered_map<QString, RootItemMetadata> m_metadata;
  std::vector<std::unique_ptr<RootProvider>> m_providers;
  OmniDatabase &m_db;

  void loadMetadata(const QString &id) {
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		SELECT
			provider_id, enabled, fallback, aliases, visit_count, last_visited_at
		FROM
			root_provider_item
	)");
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
			aliases JSON DEFAULT '{}',
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

    return true;
  }

  void indexItem(const std::shared_ptr<RootItem> &item) {
    auto metadata = itemMetadata(item->uniqueId());
    std::string name = item->displayName().toStdString();
    std::string subtitle = item->subtitle().toStdString();

    m_trie.index(name, item);
    m_trie.indexLatinText(name, item);
    m_trie.indexLatinText(subtitle, item);

    std::ranges::for_each(metadata.aliases,
                          [&](const QString &alias) { m_trie.index(alias.toStdString(), item); });
    std::ranges::for_each(item->keywords(),
                          [&](const QString &keyword) { m_trie.index(keyword.toStdString(), item); });
  }

  void rebuildTrie() {
    Timer timer;
    m_trie.clear();
    std::ranges::for_each(m_items, std::bind_front(&RootItemManager::indexItem, this));
    timer.time("trie rebuild");
  }

  void reloadProviders() {
    m_items = m_providers | std::views::transform([](const auto &p) { return p->loadItems(); }) |
              std::views::join | std::ranges::to<std::vector>();
    rebuildTrie();
    emit itemsChanged();
  }

  RootItemMetadata itemMetadata(const QString &id) const {
    if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }

    return {};
  }

public:
  RootItemManager(OmniDatabase &db) : m_db(db) { createTables(); }

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

  std::vector<std::shared_ptr<RootItem>> prefixSearch(const QString &query) {
    auto items = m_trie.prefixSearch(query.toStdString());

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
};
