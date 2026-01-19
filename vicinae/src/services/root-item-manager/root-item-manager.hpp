#pragma once
#include "argument.hpp"
#include "common.hpp"
#include "config/config.hpp"
#include "common/entrypoint.hpp"
#include "navigation-controller.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/local-storage/scoped-local-storage.hpp"
#include "services/root-item-manager/visit-tracker.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include "settings/provider-settings-detail.hpp"
#include <cstdint>
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
  bool includeFavorites = true;
  bool prioritizeAliased = true;
  std::optional<std::string> providerId;
};

class RootItem {
public:
  virtual ~RootItem() = default;

  virtual EntrypointId uniqueId() const = 0;

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
                                                           const RootItemMetadata &metadata) const {
    return {};
  }

  /**
   * Action panel shown when this item is used as a fallback command.
   */
  virtual std::unique_ptr<ActionPanelState> fallbackActionPanel(ApplicationContext *ctx,
                                                                const RootItemMetadata &metadata) const {
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
  enum Type : std::uint8_t {
    ExtensionProvider, // a collection of commands
    GroupProvider,     // a collection of other things
  };

  RootProvider() = default;
  virtual ~RootProvider() = default;

  /**
   * A provider flagged as transient will not list its items in the settings window
   * because the items are considered to be volatile by nature.
   * This applies to windows, browser tabs...
   */
  virtual bool isTransient() const { return false; }

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
  bool enabled = true;
  bool favorite = false;
  bool fallback = false;
  std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastVisitedAt;
  std::optional<std::string> alias;
  std::string providerId;
  std::shared_ptr<RootItem> item;
};

class RootItemManager : public QObject {
  Q_OBJECT

signals:
  void itemsChanged() const;
  void itemRankingReset(const EntrypointId &id) const;
  void itemFavoriteChanged(const EntrypointId &id, bool favorite) const;
  void fallbackEnabled(const EntrypointId &id) const;
  void fallbackOrderChanged(const EntrypointId &id) const;
  void fallbackDisabled(const EntrypointId &id) const;

  /**
   * Some item metadata changed.
   */
  void metadataChanged() const;

  // An item subtitle was modified, which means a refresh of the widget may be
  // required to pick up on the change.
  void subtitleChanged() const;

public:
  using ItemPtr = std::shared_ptr<RootItem>;
  using ItemList = std::vector<ItemPtr>;

  struct SearchableRootItem {
    std::shared_ptr<RootItem> item;
    std::string title;
    std::string subtitle;
    std::vector<std::string> keywords;
    RootItemMetadata *meta = nullptr;

    float fuzzyScore(std::string_view pattern = "") const;
  };

  struct ScoredItem {
    RootItemMetadata *meta = nullptr;
    double score = 0;
    // we return the shared ptr so that callers can keep a ref to it if needed,
    // but we don't want to increment the ref count as part of the search process.
    std::reference_wrapper<ItemPtr> item;
  };

  RootItemManager(config::Manager &config, LocalStorageService &storage);

  static glz::generic::object_t transformPreferenceValues(const QJsonObject &preferences);
  static QJsonObject transformPreferenceValues(const glz::generic::object_t &preferences);

  RootProvider *findProviderById(const QString &id) const;
  bool setProviderPreferenceValues(const QString &id, const QJsonObject &preferences);

  bool setItemEnabled(const EntrypointId &id, bool value);
  bool setItemPreferenceValues(const EntrypointId &id, const QJsonObject &preferences);

  void setPreferenceValues(const EntrypointId &id, const QJsonObject &preferences);

  bool setAlias(const EntrypointId &id, std::string_view alias);

  QJsonObject getProviderPreferenceValues(const QString &id) const;
  QJsonObject getItemPreferenceValues(const EntrypointId &id) const;

  /**
   * Merge the item preferences with the provider preferences.
   */
  std::vector<Preference> getMergedItemPreferences(const EntrypointId &id) const;
  QJsonObject getPreferenceValues(const EntrypointId &id) const;
  RootItemMetadata itemMetadata(const EntrypointId &id) const;
  bool isFallback(const EntrypointId &id) const;
  bool disableFallback(const EntrypointId &id);
  bool moveFallbackDown(const EntrypointId &id);
  bool moveFallbackUp(const EntrypointId &id);
  bool enableFallback(const EntrypointId &id);
  double computeScore(const RootItemMetadata &meta, int weight) const;
  double computeRecencyScore(const RootItemMetadata &meta) const;
  std::vector<std::shared_ptr<RootItem>> queryFavorites(std::optional<int> limit = {});
  std::vector<SearchableRootItem> querySuggestions(int limit = 5);
  bool resetRanking(const EntrypointId &id);
  bool registerVisit(const EntrypointId &id);
  bool setItemAsFavorite(const EntrypointId &item, bool value = true);
  bool setProviderEnabled(const QString &providerId, bool value);
  bool disableItem(const EntrypointId &id);

  bool enableItem(const EntrypointId &id);

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

  RootProvider *provider(std::string_view id) const;
  std::vector<SearchableRootItem> allItems() const { return m_items; }
  std::vector<std::shared_ptr<RootItem>> fallbackItems() const;

  /**
   * Fuzzy search across all available root items, if option to include explicitly disabled items
   * as part of the results.
   */
  void search(const QString &query, std::vector<ScoredItem> &results,
              const RootItemPrefixSearchOptions &opts = {});
  std::vector<ScoredItem> search(const QString &query, const RootItemPrefixSearchOptions &opts = {});

  RootItem *findItemById(const EntrypointId &id) const;
  bool pruneProvider(const QString &id);

private:
  static QString getEntrypointSecretPreferenceKey(const EntrypointId &id, const QString &prefName);
  void setEntrypointSecretPreference(const EntrypointId &id, const QString &prefName,
                                     const QJsonValue &value);
  QJsonValue getEntrypointSecretPreference(const EntrypointId &entrypoint, const QString &prefName) const;
  QJsonValue getProviderSecretPreference(const QString &providerId, const QString &prefName) const;
  void setProviderSecretPreference(const QString &id, const QString &prefName, const QJsonValue &value);

  ScopedLocalStorage getProviderSecretStorage(const QString &providerId) const;

  void mergeConfigWithMetadata(const config::ConfigValue &cfg);

  std::vector<std::shared_ptr<RootItem>>
  getFromSerializedEntrypointIds(std::span<const std::string> ids) const;

  std::unordered_map<EntrypointId, RootItemMetadata> m_metadata;
  std::vector<std::unique_ptr<RootProvider>> m_providers;
  config::Manager &m_cfg;
  LocalStorageService &m_storage;
  std::vector<SearchableRootItem> m_items;
  VisitTracker m_visitTracker;
};
