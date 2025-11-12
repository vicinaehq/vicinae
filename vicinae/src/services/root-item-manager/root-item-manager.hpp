#pragma once
#include "argument.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "omni-database.hpp"
#include "../../ui/image/url.hpp"
#include "preference.hpp"
#include "settings/provider-settings-detail.hpp"
#include <qdnslookup.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qhash.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class RootItemMetadata;
class ExtensionRootProvider;

struct RootItemPrefixSearchOptions {
  bool includeDisabled = false;
};

class RootItem {
public:
  virtual ~RootItem() = default;

  virtual QString providerId() const = 0;

  virtual QString uniqueId() const = 0;

  /**
   * If true, the 'space' key can be used to activate the item in the root search
   * if the alias starts with the current search text.
   */
  virtual bool supportsAliasSpaceShortcut() const { return false; }

  /**
   * The name of the item as it will be shown in the root menu.
   * This name is indexed by splitting it in multiple tokens at
   * word boundaries (assuming latin text).
   *
   * Note that camel case names are indexed as multiple tokens and not a single word.
   */
  virtual QString displayName() const = 0;

  virtual ImageURL iconUrl() const = 0;

  virtual QWidget *settingsDetail(const QJsonObject &preferences) const { return new QWidget(); }

  /**
   * Whether the item can be selected as a fallback command or not
   */
  virtual bool isSuitableForFallback() const { return false; }

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
  virtual std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                           const RootItemMetadata &metadata) {
    return {};
  }

  /**
   * Action panel shown when this item is used as a fallback command.
   */
  virtual std::unique_ptr<ActionPanelState> fallbackActionPanel(ApplicationContext *ctx,
                                                                const RootItemMetadata &metadata) {
    return {};
  }

  virtual bool isDefaultDisabled() const { return false; }

  /**
   * Additional strings that will be indexed and prefix searchable.
   */
  virtual std::vector<QString> keywords() const { return {}; }

  virtual void preferenceValuesChanged(const QJsonObject &values) const {}

  /**
   * Whether the itme should be marked as currently active. All this does is add a small dot as an
   * right below the icon.
   * This is typically used by apps that currently have active windows opened.
   */
  virtual bool isActive() const { return false; }
};

class RootProvider : public QObject {
  Q_OBJECT

signals:
  void itemsChanged() const;
  void itemRemoved(const QString &id) const;

public:
  enum Type {
    ExtensionProvider, // a collection of commands
    GroupProvider,     // a collection of other things
  };

  RootProvider() {}
  virtual ~RootProvider() = default;

  virtual QString uniqueId() const = 0;
  virtual QString displayName() const = 0;
  virtual ImageURL icon() const = 0;
  virtual Type type() const = 0;

  QString typeAsString() {
    switch (type()) {
    case ExtensionProvider:
      return "Extension";
    case GroupProvider:
      return "Group";
    default:
      return "Unknown";
    }
  }

  bool isExtension() const { return type() == Type::ExtensionProvider; }
  bool isGroup() const { return type() == Type::GroupProvider; }

  /**
   * Generate the default set of preferences for this item.
   * This function is called on _each_ startup and diffed against the existing preference values.
   * Existing keys are untouched but new ones can be added.
   */
  virtual QJsonObject generateDefaultPreferences() const { return {}; }

  /**
   * Called when the provider preferences are changed.
   */
  virtual void preferencesChanged(const QJsonObject &preferences) {}

  virtual void itemPreferencesChanged(const QString &itemId, const QJsonObject &preferences) {}

  virtual QWidget *settingsDetail() const { return new ProviderSettingsDetail(*this); }

  // Called the first time the root provider is loaded by the root item manager, right after the first
  // `preferencesChanged` call.
  virtual void initialized(const QJsonObject &preference) {}

  /**
   * Called only once, right after `initialized`. This can be used to transform the preference object and save
   * the changes in database right away. This can be useful to implement migrations from a version to another.
   */
  virtual std::optional<QJsonObject> patchPreferences(const QJsonObject &values) { return {}; }

  virtual std::vector<std::shared_ptr<RootItem>> loadItems() const = 0;
  virtual PreferenceList preferences() const { return {}; }
};

struct RootItemMetadata {
  int visitCount = 0;
  bool isEnabled = true;
  std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastVisitedAt;
  // Alias can be made of multiple words, in which case each word is indexed separately
  std::string alias; // std::string cause we want it to be fuzzy searchready
  bool favorite = false;
  int fallbackPosition = -1;
  QString providerId;
  bool isFallback() const { return fallbackPosition != -1; }
};

struct RootProviderMetadata {
  bool enabled;
};

class RootItemManager : public QObject {
  Q_OBJECT

signals:
  void itemsChanged() const;
  void itemRankingReset(const QString &id) const;
  void itemFavoriteChanged(const QString &id, bool favorite);
  void fallbackEnabled(const QString &id) const;
  void fallbackOrderChanged(const QString &id) const;
  void fallbackDisabled(const QString &id) const;

public:
  using ItemPtr = std::shared_ptr<RootItem>;
  using ItemList = std::vector<ItemPtr>;
  struct SearchableRootItem {
    std::shared_ptr<RootItem> item;
    std::string title;
    std::string subtitle;
    std::vector<std::string> keywords;
    RootItemMetadata *meta;

    int fuzzyScore(std::string_view pattern) const;
  };

  struct ScoredItem {
    std::string_view alias;
    int score = 0;
    // we return the shared ptr so that callers can keep a ref to it if needed,
    // but we don't want to increment the ref count as part of the search process.
    std::reference_wrapper<ItemPtr> item;
  };

  RootItemManager(OmniDatabase &db) : m_db(db) {}

  RootProvider *findProviderById(const QString &id) const;
  bool setProviderPreferenceValues(const QString &id, const QJsonObject &preferences);

  bool setItemEnabled(const QString &id, bool value);
  bool setItemPreferenceValues(const QString &id, const QJsonObject &preferences);

  /**
   * Set preference values for the item with _id_.
   * This method dissociates preference values that belong to the provider's preferences from
   * those that belong to the item with _id_.
   */
  void setPreferenceValues(const QString &id, const QJsonObject &preferences);

  bool setAlias(const QString &id, const QString &alias);
  bool clearAlias(const QString &id);

  QJsonObject getProviderPreferenceValues(const QString &id) const;
  QJsonObject getItemPreferenceValues(const QString &id) const;

  /**
   * Merge the item preferences with the provider preferences.
   */
  std::vector<Preference> getMergedItemPreferences(const QString &rootItemId) const;
  QJsonObject getPreferenceValues(const QString &id) const;
  RootItemMetadata itemMetadata(const QString &id) const;
  int maxFallbackPosition();
  bool isFallback(const QString &id) const;
  bool disableFallback(const QString &id);
  bool moveFallbackDown(const QString &id);
  bool moveFallbackUp(const QString &id);
  bool enableFallback(const QString &id);
  double computeScore(const RootItemMetadata &meta, int weight) const;
  double computeRecencyScore(const RootItemMetadata &meta) const;
  std::vector<SearchableRootItem> queryFavorites(std::optional<int> limit = {});
  std::vector<SearchableRootItem> querySuggestions(int limit = 5);
  bool resetRanking(const QString &id);
  bool registerVisit(const QString &id);
  bool setItemAsFavorite(const QString &item, bool value = true);
  QString getItemProviderId(const QString &id);
  bool setProviderEnabled(const QString &providerId, bool value);
  bool disableItem(const QString &id);

  bool enableItem(const QString &id);

  std::vector<RootProvider *> providers() const;
  std::vector<ExtensionRootProvider *> extensions() const;

  void updateIndex();

  /**
   * DESTRUCTIVE!
   * This will unload the provider AND wipe persisted data such as aliases, preferences, etc...
   */
  void uninstallProvider(const QString &id);

  /**
   * Unload provider from the tracked list of providers.
   * You need to call reloadProviders() once you are done making changes in order
   * to cleanup the old items from the index.
   */
  void unloadProvider(const QString &id);

  void loadProvider(std::unique_ptr<RootProvider> provider);

  RootProvider *provider(const QString &id) const;
  std::vector<SearchableRootItem> allItems() const { return m_items; }
  std::vector<std::shared_ptr<RootItem>> fallbackItems() const;

  /**
   * Fuzzy search across all available root items, if option to include explicitly disabled items
   * as part of the results.
   */
  std::span<ScoredItem> search(const QString &query, const RootItemPrefixSearchOptions &opts = {});

  RootItemMetadata loadMetadata(const QString &id);
  bool upsertProvider(const RootProvider &provider);
  bool upsertItem(const QString &providerId, const RootItem &item);
  RootItem *findItemById(const QString &id) const;
  SearchableRootItem *findSearchableItem(const QString &id);
  bool pruneProvider(const QString &id);

private:
  std::unordered_map<QString, RootItemMetadata> m_metadata;
  std::unordered_map<QString, RootProviderMetadata> m_provider_metadata;
  std::vector<std::unique_ptr<RootProvider>> m_providers;
  OmniDatabase &m_db;
  std::vector<SearchableRootItem> m_items;

  // always reserved to hold the maximum amount of items possible, to avoid reallocating
  // on every search
  std::vector<ScoredItem> m_scoredItems;
};
