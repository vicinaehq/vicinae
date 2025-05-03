#pragma once
#include "argument.hpp"
#include "libtrie/trie.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/default-list-item-widget.hpp"
#include <algorithm>
#include <qdnslookup.h>
#include <qnamespace.h>
#include <qobject.h>
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
  RootProvider() {}
  virtual ~RootProvider() = default;

  virtual QString displayName() const = 0;
  virtual std::vector<std::shared_ptr<RootItem>> loadItems() const = 0;

signals:
  void itemsChanged() const;
};

class RootItemManager : public QObject {
public:
private:
  using ItemPtr = std::shared_ptr<RootItem>;
  using ItemList = std::vector<ItemPtr>;

  struct RootItemHash {
    size_t operator()(const std::shared_ptr<RootItem> &item) const { return qHash(item->uniqueId()); }
  };

  struct RootItemMetadata {
    int openCount;
    bool isEnabled;
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastOpenedAt;
    std::vector<QString> aliases;
  };

  Trie<std::shared_ptr<RootItem>, RootItemHash> m_trie;
  std::vector<std::shared_ptr<RootItem>> m_items;
  std::unordered_map<QString, RootItemMetadata> m_metadata;
  std::vector<std::unique_ptr<RootProvider>> m_providers;

  void rebuildTrie() {
    m_trie.clear();

    for (const auto &item : m_items) {
      auto metadata = itemMetadata(item->uniqueId());
      std::string name = item->displayName().toStdString();

      m_trie.index(name, item);
      m_trie.indexLatinText(name, item);

      for (const auto &alias : metadata.aliases) {
        m_trie.index(alias.toStdString(), item);
      }

      for (const auto &keyword : item->keywords()) {
        m_trie.index(keyword.toStdString(), item);
      }
    }
  }

  void reloadProviders() {
    m_items = m_providers | std::views::transform([](const auto &p) { return p->loadItems(); }) |
              std::views::join | std::ranges::to<std::vector>();
    rebuildTrie();
  }

  RootItemMetadata itemMetadata(const QString &id) const {
    if (auto it = m_metadata.find(id); it != m_metadata.end()) { return it->second; }

    return {};
  }

public:
  std::vector<RootProvider *> providers() const {
    return m_providers | std::views::transform([](const auto &p) { return p.get(); }) |
           std::ranges::to<std::vector>();
  }

  void addProvider(std::unique_ptr<RootProvider> provider) {
    auto items = provider->loadItems();

    m_items.insert(m_items.end(), items.begin(), items.end());
    connect(provider.get(), &RootProvider::itemsChanged, this, &RootItemManager::reloadProviders);
    m_providers.emplace_back(std::move(provider));
    rebuildTrie();
  }

  std::vector<std::shared_ptr<RootItem>> prefixSearch(const QString &query) {
    auto items = m_trie.prefixSearch(query.toStdString()) | std::views::filter([this](const auto &a) {
                   return true;
                   return itemMetadata(a->uniqueId()).isEnabled;
                 }) |
                 std::ranges::to<std::vector>();

    std::ranges::sort(items, [this](const auto &a, const auto &b) {
      auto ameta = itemMetadata(a->uniqueId());
      auto bmeta = itemMetadata(b->uniqueId());

      return bmeta.openCount > ameta.openCount;
    });

    return items;
  }
};
