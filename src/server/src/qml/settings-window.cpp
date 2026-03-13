#include "settings-window.hpp"
#include "common/entrypoint.hpp"
#include "async-image-provider.hpp"
#include "config-bridge.hpp"
#include "extension-settings-model.hpp"
#include "general-settings-model.hpp"
#include "image-source.hpp"
#include "keybind-settings-model.hpp"
#include "theme-bridge.hpp"
#include "view-utils.hpp"
#include "config/config.hpp"
#include "extension/extension.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "service-registry.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/app-service/app-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings-controller/settings-controller.hpp"
#include "vicinae.hpp"
#include "version.h"
#include "lib/fuzzy/fuzzy-searchable.hpp"
#include <QQmlContext>
#include <QQuickWindow>

SettingsWindow::SettingsWindow(ApplicationContext &ctx, QObject *parent) : QObject(parent), m_ctx(ctx) {}

void SettingsWindow::ensureInitialized() {
  if (m_initialized) return;
  m_initialized = true;

  m_themeBridge = new ThemeBridge(this);
  m_configBridge = new ConfigBridge(this);
  m_imgSource = new ImageSource(this);
  m_generalModel = new GeneralSettingsModel(this);
  m_keybindModel = new KeybindSettingsModel(this);
  m_extensionModel = new ExtensionSettingsModel(this);

  m_engine.addImageProvider(QStringLiteral("vicinae"), new AsyncImageProvider());

  auto *rootCtx = m_engine.rootContext();
  rootCtx->setContextProperty(QStringLiteral("Theme"), m_themeBridge);
  rootCtx->setContextProperty(QStringLiteral("Config"), m_configBridge);
  rootCtx->setContextProperty(QStringLiteral("Img"), m_imgSource);
  rootCtx->setContextProperty(QStringLiteral("settings"), this);
  rootCtx->setContextProperty(QStringLiteral("FileChooser"),
                              ServiceRegistry::instance()->fileChooserService());

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  connect(manager, &RootItemManager::itemsChanged, this, &SettingsWindow::rebuildSidebarExtensions);
  connect(m_extensionModel, &ExtensionSettingsModel::providerEnabledChanged, this,
          &SettingsWindow::rebuildSidebarExtensions);
  rebuildSidebarExtensions();

  m_engine.load(QUrl(QStringLiteral("qrc:/Vicinae/SettingsWindow.qml")));

  auto rootObjects = m_engine.rootObjects();
  if (!rootObjects.isEmpty()) { m_window = qobject_cast<QQuickWindow *>(rootObjects.first()); }

  if (m_window) {
    connect(m_window, &QQuickWindow::visibleChanged, this, [this](bool visible) {
      if (!visible) m_ctx.settings->closeWindow();
    });
    connect(m_window, &QQuickWindow::widthChanged, this, &SettingsWindow::updateBlur);
    connect(m_window, &QQuickWindow::heightChanged, this, &SettingsWindow::updateBlur);
  }

  connect(m_ctx.services->config(), &config::Manager::configChanged, this, &SettingsWindow::updateBlur);
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
    entry[QStringLiteral("provenance")] = ExtensionSettingsModel::provenanceForProvider(provider);
    m_sidebarExtensions.append(entry);
  }
  emit sidebarExtensionsChanged();
}

QString SettingsWindow::version() const { return QStringLiteral(VICINAE_GIT_TAG); }
QString SettingsWindow::commitHash() const { return QStringLiteral(VICINAE_GIT_COMMIT_HASH); }
QString SettingsWindow::buildInfo() const { return QStringLiteral(BUILD_INFO); }
QString SettingsWindow::headline() const { return Omnicast::HEADLINE; }

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
  updateBlur();
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
    QString provenance;
    bool isGroup = false;
    bool enabled = true;
  };

  static const std::array corePages = {
      SidebarEntry{.id = QStringLiteral("general"),
                   .label = QStringLiteral("General"),
                   .icon = QStringLiteral("cog"),
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
        .provenance = map[QStringLiteral("provenance")].toString(),
        .isGroup = isGroup,
        .enabled = map[QStringLiteral("enabled")].toBool(),
    });
  }

  auto queryStd = query.toStdString();

  struct ScoredEntry {
    const SidebarEntry *entry;
    int score;
  };

  std::vector<ScoredEntry> scored;
  scored.reserve(all.size());

  for (const auto &e : all) {
    if (queryStd.empty()) {
      scored.push_back({&e, 0});
    } else {
      auto label = e.label.toStdString();
      int s = fuzzy::scoreWeighted({{label, 1.0}}, queryStd);
      if (s > 0) { scored.push_back({&e, s}); }
    }
  }

  if (!queryStd.empty()) {
    std::stable_sort(scored.begin(), scored.end(),
                     [](const auto &a, const auto &b) { return a.score > b.score; });
  }

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
      m[QStringLiteral("provenance")] = e.provenance;
    }
    return m;
  };

  QVariantList result;
  if (queryStd.empty()) {
    bool hasCore = false;
    bool hasGroups = false;
    bool hasExts = false;

    for (const auto &s : scored) {
      if (s.entry->kind == kCore)
        hasCore = true;
      else if (s.entry->isGroup)
        hasGroups = true;
      else
        hasExts = true;
    }

    for (const auto &s : scored) {
      if (s.entry->kind != kCore) break;
      result.append(makeEntry(*s.entry));
    }

    if (hasCore && (hasGroups || hasExts)) {
      QVariantMap div;
      div[kKind] = kDivider;
      result.append(div);
    }

    for (const auto &s : scored) {
      if (!s.entry->isGroup) continue;
      result.append(makeEntry(*s.entry));
    }

    if (hasGroups && hasExts) {
      QVariantMap div;
      div[kKind] = kDivider;
      result.append(div);
    }

    for (const auto &s : scored) {
      if (s.entry->kind != kExt) continue;
      result.append(makeEntry(*s.entry));
    }
  } else {
    for (const auto &s : scored) {
      result.append(makeEntry(*s.entry));
    }
  }

  return result;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

void SettingsWindow::updateBlur() {
  if (!m_window) return;
  auto &cfg = m_ctx.services->config()->value();
  auto *bgEffect = m_ctx.services->backgroundEffectManager();
  if (!bgEffect->supportsBlur()) return;

  if (cfg.launcherWindow.blur.enabled) {
    QRect const region(0, 0, m_window->width(), m_window->height());
    bgEffect->setBlur(m_window, {.radius = 10, .region = region});
  } else {
    bgEffect->clearBlur(m_window);
  }
}
