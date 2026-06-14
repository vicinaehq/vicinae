#include "settings-window.hpp"
#include "common/entrypoint.hpp"
#include "config-bridge.hpp"
#include "environment.hpp"
#include "extension-settings-model.hpp"
#include "general-settings-model.hpp"
#include "image-source.hpp"
#include "keyboard-bridge.hpp"
#include "global-shortcut-bridge.hpp"
#include "keybind-settings-model.hpp"
#include "theme-bridge.hpp"
#include "view-utils.hpp"
#include "config/config.hpp"
#include "extension/extension.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/app-service/app-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings-controller/settings-controller.hpp"
#include "vicinae.hpp"
#include "generated/version.h"
#include "fuzzy/fuzzy-searchable.hpp"
#include <QQmlContext>
#include <QQuickWindow>
#ifdef Q_OS_MACOS
#include "macos-chrome-attached.hpp"
#endif

SettingsWindow::SettingsWindow(ApplicationContext &ctx, QObject *parent) : QObject(parent), m_ctx(ctx) {}

void SettingsWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;

  m_themeBridge = new ThemeBridge(this);
  m_configBridge = new ConfigBridge(this);
  m_imgSource = new ImageSource(this);
  m_keyboardBridge = new KeyboardBridge(this);
  m_globalShortcutBridge = new GlobalShortcutBridge(this);
  m_generalModel = new GeneralSettingsModel(this);
  m_keybindModel = new KeybindSettingsModel(this);
  m_extensionModel = new ExtensionSettingsModel(this);

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);
  rootCtx->setContextProperty(QStringLiteral("Keyboard"), m_keyboardBridge);
  rootCtx->setContextProperty(QStringLiteral("GlobalShortcuts"), m_globalShortcutBridge);
  rootCtx->setContextProperty(QStringLiteral("settings"), this);
  rootCtx->setContextProperty(QStringLiteral("FileChooser"),
                              ServiceRegistry::instance()->fileChooserService());

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  connect(manager, &RootItemManager::itemsChanged, this, &SettingsWindow::rebuildSidebarExtensions);
  connect(m_extensionModel, &ExtensionSettingsModel::providerEnabledChanged, this,
          &SettingsWindow::updateSidebarEnabled);
  rebuildSidebarExtensions();

  m_engine.load(QUrl(
#ifdef Q_OS_MACOS
      QStringLiteral("qrc:/Vicinae/SettingsWindowMacOS.qml")
#else
      QStringLiteral("qrc:/Vicinae/SettingsWindow.qml")
#endif
          ));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

  if (m_window) {
    connect(m_window, &QQuickWindow::visibleChanged, this, [this](bool visible) {
      if (!visible) m_ctx.settings->closeWindow();
    });
  }
}

void SettingsWindow::setCurrentPage(const QString &page) {
  if (m_currentPage != page) {
    m_currentPage = page;
    emit currentPageChanged();
  }
}

void SettingsWindow::setPendingCommandId(const QString &id) {
  if (m_pendingCommandId != id) {
    m_pendingCommandId = id;
    emit pendingCommandIdChanged();
  }
}

QVariantList SettingsWindow::sidebarExtensions() const { return m_sidebarExtensions; }

void SettingsWindow::rebuildSidebarExtensions() {
  m_sidebarExtensions.clear();
  auto *manager = ServiceRegistry::instance()->rootItemManager();
  auto &cfg = m_ctx.services->config()->value();
  for (auto *provider : manager->providers()) {
    if (provider->isTransient()) continue;
    auto id = provider->uniqueId().toStdString();
    bool enabled = true;
    if (auto it = cfg.providers.find(id); it != cfg.providers.end()) {
      enabled = it->second.enabled.value_or(true);
    }
    QVariantMap entry;
    entry[QStringLiteral("name")] = provider->displayName();
    entry[QStringLiteral("iconSource")] = qml::imageSourceFor(provider->icon());
    entry[QStringLiteral("providerId")] = provider->uniqueId();
    entry[QStringLiteral("isGroup")] = provider->isGroup();
    entry[QStringLiteral("enabled")] = enabled;
    m_sidebarExtensions.append(entry);
  }

  m_providerCommands.clear();
  for (const auto &si : manager->allItems()) {
    if (!si.item) continue;
    auto id = si.item->uniqueId();
    m_providerCommands[id.provider].push_back({.entrypointId = std::string(id),
                                               .name = si.item->title().toStdString(),
                                               .iconSource = qml::imageSourceFor(si.item->iconUrl())});
  }

  emit sidebarExtensionsChanged();
}

void SettingsWindow::updateSidebarEnabled(const QString &providerId, bool enabled) {
  for (auto &ext : m_sidebarExtensions) {
    auto map = ext.toMap();
    if (map[QStringLiteral("providerId")].toString() == providerId) {
      map[QStringLiteral("enabled")] = enabled;
      ext = map;
      break;
    }
  }
  emit sidebarItemEnabledChanged(providerId, enabled);
}

QString SettingsWindow::version() const { return QStringLiteral(VICINAE_GIT_TAG); }
QString SettingsWindow::commitHash() const { return QStringLiteral(VICINAE_GIT_COMMIT_HASH); }
QString SettingsWindow::buildInfo() const { return QStringLiteral(BUILD_INFO); }
QString SettingsWindow::headline() const { return Omnicast::HEADLINE; }

bool SettingsWindow::globalShortcutsSupported() const {
  auto *service = ServiceRegistry::instance()->globalShortcuts();
  return service && service->isSupported();
}

bool SettingsWindow::layerShellSupported() const { return Environment::isLayerShellSupported(); }

void SettingsWindow::openUrl(const QString &url) { m_ctx.services->appDb()->openTarget(url); }

void SettingsWindow::close() {
  if (m_window) m_window->hide();
}

void SettingsWindow::requestDefaultFocus() { emit defaultFocusRequested(); }

void SettingsWindow::show() {
  ensureInitialized();
  if (!m_window) return;
  m_window->show();
  m_window->raise();
  m_window->requestActivate();
#ifdef Q_OS_MACOS
  macosActivateApp();
#endif
}

void SettingsWindow::hide() {
  if (m_window) m_window->hide();
}

void SettingsWindow::openTab(const QString &tabId) {
  ensureInitialized();
  if (tabId == "keybinds" || tabId == "shortcuts") {
    setCurrentPage(QStringLiteral("keybindings"));
  } else if (tabId == "extensions") {
    setCurrentPage(QStringLiteral("general"));
  } else {
    setCurrentPage(tabId);
  }
}

void SettingsWindow::selectExtension(const QString &entrypointId) {
  ensureInitialized();
  auto providerId = QString::fromStdString(EntrypointId::fromSerialized(entrypointId.toStdString()).provider);
  if (providerId.isEmpty()) return;
  m_extensionModel->selectProviderById(providerId);
  // Page first so the surviving (new) page handles the pending command.
  setCurrentPage(providerId);
  setPendingCommandId(entrypointId);
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
QVariantList SettingsWindow::filterSidebarItems(const QString &query) const {
  static const auto kId = QStringLiteral("id");
  static const auto kLabel = QStringLiteral("label");
  static const auto kIcon = QStringLiteral("icon");
  static const auto kIconSource = QStringLiteral("iconSource");
  static const auto kKind = QStringLiteral("_kind");
  static const auto kCore = QStringLiteral("core");
  static const auto kGroup = QStringLiteral("group");
  static const auto kExt = QStringLiteral("ext");
  static const auto kDivider = QStringLiteral("divider");

  struct SidebarEntry {
    QString id;
    QString label;
    QString icon;
    QString iconSource;
    QString kind;
    bool isGroup = false;
    bool enabled = true;
  };

  static const std::array corePages = {
      SidebarEntry{.id = QStringLiteral("general"),
                   .label = QStringLiteral("General"),
                   .icon = QStringLiteral("cog"),
                   .kind = kCore},
      SidebarEntry{.id = QStringLiteral("appearance"),
                   .label = QStringLiteral("Appearance"),
                   .icon = QStringLiteral("swatch"),
                   .kind = kCore},
      SidebarEntry{.id = QStringLiteral("keybindings"),
                   .label = QStringLiteral("Keybindings"),
                   .icon = QStringLiteral("keyboard"),
                   .kind = kCore},
      SidebarEntry{.id = QStringLiteral("advanced"),
                   .label = QStringLiteral("Advanced"),
                   .icon = QStringLiteral("wrench-screwdriver"),
                   .kind = kCore},
      SidebarEntry{.id = QStringLiteral("about"),
                   .label = QStringLiteral("About"),
                   .icon = QStringLiteral("vicinae"),
                   .kind = kCore},
  };

  std::vector<SidebarEntry> all;
  all.reserve(corePages.size() + m_sidebarExtensions.size());

  for (const auto &page : corePages) {
    all.push_back(page);
  }
  for (const auto &ext : m_sidebarExtensions) {
    auto map = ext.toMap();
    auto isGroup = map[QStringLiteral("isGroup")].toBool();
    all.push_back({
        .id = map[QStringLiteral("providerId")].toString(),
        .label = map[QStringLiteral("name")].toString(),
        .iconSource = map[kIconSource].toString(),
        .kind = isGroup ? kGroup : kExt,
        .isGroup = isGroup,
        .enabled = map[QStringLiteral("enabled")].toBool(),
    });
  }

  auto queryStd = query.toStdString();

  static const auto kEnabled = QStringLiteral("enabled");

  auto makeEntry = [&](const SidebarEntry &e) {
    QVariantMap m;
    m[kId] = e.id;
    m[kLabel] = e.label;
    m[kKind] = e.kind;
    m[kEnabled] = e.enabled;
    if (e.kind == kCore) {
      m[kIcon] = e.icon;
    } else {
      m[kIconSource] = e.iconSource;
    }
    return m;
  };

  QVariantList result;
  if (queryStd.empty()) {
    bool hasCore = false;
    bool hasGroups = false;
    bool hasExts = false;

    for (const auto &e : all) {
      if (e.kind == kCore)
        hasCore = true;
      else if (e.isGroup)
        hasGroups = true;
      else
        hasExts = true;
    }

    for (const auto &e : all) {
      if (e.kind != kCore) break;
      result.append(makeEntry(e));
    }

    if (hasCore && (hasGroups || hasExts)) {
      QVariantMap div;
      div[kKind] = kDivider;
      result.append(div);
    }

    for (const auto &e : all) {
      if (!e.isGroup) continue;
      result.append(makeEntry(e));
    }

    if (hasGroups && hasExts) {
      QVariantMap div;
      div[kKind] = kDivider;
      result.append(div);
    }

    for (const auto &e : all) {
      if (e.kind != kExt) continue;
      result.append(makeEntry(e));
    }
  } else {
    static const auto kCommand = QStringLiteral("command");
    auto *manager = ServiceRegistry::instance()->rootItemManager();

    auto makeCommandEntry = [&](const QString &id, const QString &name, const QString &iconSource) {
      QVariantMap m;
      m[kId] = id;
      m[kLabel] = name;
      m[kKind] = kCommand;
      m[kIconSource] = iconSource;
      m[kEnabled] = true;
      return m;
    };

    struct MatchedCommand {
      QString id;
      QString name;
      QString iconSource;
    };
    std::unordered_map<std::string, std::vector<MatchedCommand>> matchedByProvider;
    std::unordered_map<std::string, double> providerCmdScore;
    {
      RootItemPrefixSearchOptions opts;
      opts.includeDisabled = true;
      for (const auto &scored : manager->search(query, opts)) {
        auto &item = scored.item.get();
        if (!item) continue;
        auto id = item->uniqueId();
        matchedByProvider[id.provider].push_back(
            {QString::fromStdString(std::string(id)), item->title(), qml::imageSourceFor(item->iconUrl())});
        auto &best = providerCmdScore[id.provider];
        best = std::max(best, scored.score);
      }
    }

    struct TopLevel {
      const SidebarEntry *entry;
      double score;
      QVariantList commands;
    };

    std::vector<TopLevel> tops;
    tops.reserve(all.size());

    for (const auto &e : all) {
      int const nameScore = fuzzy::scoreWeighted({{e.label.toStdString(), 1.0}}, queryStd);
      bool const nameMatched = nameScore > 0;

      if (e.kind == kCore) {
        if (nameMatched) tops.push_back({&e, static_cast<double>(nameScore), {}});
        continue;
      }

      auto providerId = e.id.toStdString();
      QVariantList commands;
      double cmdScore = 0;

      if (nameMatched) {
        // A name match lists all the provider's commands (search only matches an
        // item's own text).
        if (auto it = m_providerCommands.find(providerId); it != m_providerCommands.end()) {
          for (const auto &c : it->second) {
            commands.append(makeCommandEntry(QString::fromStdString(c.entrypointId),
                                             QString::fromStdString(c.name), c.iconSource));
          }
        }
      } else if (auto it = matchedByProvider.find(providerId); it != matchedByProvider.end()) {
        for (const auto &c : it->second) {
          commands.append(makeCommandEntry(c.id, c.name, c.iconSource));
        }
        cmdScore = providerCmdScore[providerId];
      }

      if (nameMatched || !commands.isEmpty())
        tops.push_back({&e, nameMatched ? static_cast<double>(nameScore) : cmdScore, std::move(commands)});
    }

    std::stable_sort(tops.begin(), tops.end(),
                     [](const auto &a, const auto &b) { return a.score > b.score; });

    for (const auto &top : tops) {
      result.append(makeEntry(*top.entry));
      for (const auto &c : top.commands) {
        result.append(c);
      }
    }
  }

  return result;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
