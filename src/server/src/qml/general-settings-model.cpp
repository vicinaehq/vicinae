#include "general-settings-model.hpp"
#include "capabilities.hpp"
#include "config/config.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#ifdef Q_OS_LINUX
#include "internal/icon-theme-db/icon-theme-db.hpp"
#endif
#include <QGuiApplication>
#include <QIcon>

GeneralSettingsModel::GeneralSettingsModel(QObject *parent) : QObject(parent) {
  connect(ServiceRegistry::instance()->config(), &config::Manager::configChanged, this,
          &GeneralSettingsModel::configChanged);
}

const config::ConfigValue &GeneralSettingsModel::cfg() const {
  return ServiceRegistry::instance()->config()->value();
}

config::Manager &GeneralSettingsModel::cfgManager() const { return *ServiceRegistry::instance()->config(); }

bool GeneralSettingsModel::searchFilesInRoot() const { return cfg().search_files_in_root; }
void GeneralSettingsModel::setSearchFilesInRoot(bool v) {
  cfgManager().mergeWithUser({.search_files_in_root = v});
}

bool GeneralSettingsModel::closeOnFocusLoss() const { return cfg().close_on_focus_loss; }
void GeneralSettingsModel::setCloseOnFocusLoss(bool v) {
  cfgManager().mergeWithUser({.close_on_focus_loss = v});
}

bool GeneralSettingsModel::closeOnEscape() const { return cfg().escape_key_behavior == "close_window"; }
void GeneralSettingsModel::setCloseOnEscape(bool v) {
  cfgManager().mergeWithUser({.escape_key_behavior = v ? std::string{"close_window"} : std::string{}});
}

bool GeneralSettingsModel::considerPreedit() const { return cfg().consider_preedit; }
void GeneralSettingsModel::setConsiderPreedit(bool v) { cfgManager().mergeWithUser({.consider_preedit = v}); }

bool GeneralSettingsModel::popToRootOnClose() const { return cfg().pop_to_root_on_close; }
void GeneralSettingsModel::setPopToRootOnClose(bool v) {
  cfgManager().mergeWithUser({.pop_to_root_on_close = v});
}

bool GeneralSettingsModel::popOnBackspace() const { return cfg().pop_on_backspace; }
void GeneralSettingsModel::setPopOnBackspace(bool v) { cfgManager().mergeWithUser({.pop_on_backspace = v}); }

bool GeneralSettingsModel::activateOnSingleClick() const { return cfg().activate_on_single_click; }
void GeneralSettingsModel::setActivateOnSingleClick(bool v) {
  cfgManager().mergeWithUser({.activate_on_single_click = v});
}

bool GeneralSettingsModel::encryptSensitiveData() const { return cfg().encrypt_sensitive_data; }
void GeneralSettingsModel::setEncryptSensitiveData(bool v) {
  cfgManager().mergeWithUser({.encrypt_sensitive_data = v});
}

bool GeneralSettingsModel::telemetrySystemInfo() const { return cfg().telemetry.system_info; }
void GeneralSettingsModel::setTelemetrySystemInfo(bool v) {
  cfgManager().mergeWithUser({.telemetry = config::Partial<config::TelemetryConfig>{.system_info = v}});
}

bool GeneralSettingsModel::layerShellEnabled() const { return cfg().launcher_window.layer_shell.enabled; }
void GeneralSettingsModel::setLayerShellEnabled(bool v) {
  cfgManager().mergeWithUser({.launcher_window = config::Partial<config::WindowConfig>{
                                  .layer_shell = config::Partial<config::LayerShellConfig>{.enabled = v}}});
}

bool GeneralSettingsModel::clientSideDecorations() const {
  return cfg().launcher_window.client_side_decorations.enabled;
}
void GeneralSettingsModel::setClientSideDecorations(bool v) {
  cfgManager().mergeWithUser(
      {.launcher_window = config::Partial<config::WindowConfig>{
           .client_side_decorations = config::Partial<config::WindowCSD>{.enabled = v}}});
}

QString GeneralSettingsModel::rounding() const {
  return QString::number(cfg().launcher_window.effectiveRounding());
}
void GeneralSettingsModel::setRounding(const QString &v) {
  bool ok = false;
  int val = v.toInt(&ok);
  if (ok)
    cfgManager().mergeWithUser({.launcher_window = config::Partial<config::WindowConfig>{.rounding = val}});
}

QString GeneralSettingsModel::csdBorderWidth() const {
  return QString::number(cfg().launcher_window.client_side_decorations.border_width);
}
void GeneralSettingsModel::setCsdBorderWidth(const QString &v) {
  bool ok = false;
  int val = v.toInt(&ok);
  if (ok)
    cfgManager().mergeWithUser(
        {.launcher_window = config::Partial<config::WindowConfig>{
             .client_side_decorations = config::Partial<config::WindowCSD>{.border_width = val}}});
}

QString GeneralSettingsModel::csdShadowSize() const {
  return QString::number(cfg().launcher_window.client_side_decorations.shadow_size);
}
void GeneralSettingsModel::setCsdShadowSize(const QString &v) {
  bool ok = false;
  int val = v.toInt(&ok);
  if (ok)
    cfgManager().mergeWithUser(
        {.launcher_window = config::Partial<config::WindowConfig>{
             .client_side_decorations = config::Partial<config::WindowCSD>{.shadow_size = val}}});
}

bool GeneralSettingsModel::compactMode() const { return cfg().launcher_window.compact_mode.enabled; }
void GeneralSettingsModel::setCompactMode(bool v) {
  cfgManager().mergeWithUser({.launcher_window = config::Partial<config::WindowConfig>{
                                  .compact_mode = config::Partial<config::WindowCompactMode>{.enabled = v}}});
}

bool GeneralSettingsModel::inputServerEnabled() const { return cfg().input_server.enabled; }

void GeneralSettingsModel::setInputServerEnabled(bool v) {
  cfgManager().mergeWithUser({.input_server = config::Partial<config::InputServer>{.enabled = v}});
}

QString GeneralSettingsModel::windowOpacity() const {
  return QString::number(
      cfg().launcher_window.resolvedOpacity(platform::supports(platform::Capability::LiquidGlass),
                                           platform::supports(platform::Capability::WindowMaterial)));
}
void GeneralSettingsModel::setWindowOpacity(const QString &v) {
  bool ok = false;
  float val = v.toFloat(&ok);
  if (ok)
    cfgManager().mergeWithUser({.launcher_window = config::Partial<config::WindowConfig>{.opacity = val}});
}

bool GeneralSettingsModel::nativeTextRendering() const { return cfg().font.rendering != "qt"; }
void GeneralSettingsModel::setNativeTextRendering(bool v) {
  cfgManager().mergeWithUser({.font = config::Partial<config::FontConfig>{.rendering = v ? "native" : "qt"}});
}

QString GeneralSettingsModel::fontSize() const { return QString::number(cfg().font.normal.size); }
void GeneralSettingsModel::setFontSize(const QString &v) {
  bool ok = false;
  float val = v.toFloat(&ok);
  if (ok) cfgManager().mergeWithUser({.font = config::Partial<config::FontConfig>{.normal{.size = val}}});
}

static QVariantMap makeDropdownItem(const QString &id, const QString &displayName,
                                    const QString &iconSource = {}) {
  QVariantMap m;
  m[QStringLiteral("id")] = id;
  m[QStringLiteral("displayName")] = displayName;
  if (!iconSource.isEmpty()) m[QStringLiteral("iconSource")] = iconSource;
  return m;
}

static QVariantList wrapSection(const QString &title, const QVariantList &items) {
  QVariantMap section;
  section[QStringLiteral("title")] = title;
  section[QStringLiteral("items")] = items;
  return {section};
}

QVariantList GeneralSettingsModel::windowMaterialItems() const {
  QVariantList items;
  items.append(makeDropdownItem(QStringLiteral("none"), QStringLiteral("None")));
  items.append(makeDropdownItem(QStringLiteral("blur"), QStringLiteral("Blurred")));
  if (platform::supports(platform::Capability::LiquidGlass))
    items.append(makeDropdownItem(QStringLiteral("liquid_glass"), QStringLiteral("Liquid Glass")));
  return wrapSection(QStringLiteral("Window material"), items);
}

QVariant GeneralSettingsModel::currentWindowMaterial() const {
  auto id = QString::fromStdString(
      cfg().launcher_window.resolvedMaterial(platform::supports(platform::Capability::LiquidGlass),
                                            platform::supports(platform::Capability::WindowMaterial)));
  QString name = id == "liquid_glass" ? QStringLiteral("Liquid Glass")
                 : id == "none"       ? QStringLiteral("None")
                                      : QStringLiteral("Blurred");
  return makeDropdownItem(id, name);
}

void GeneralSettingsModel::selectWindowMaterial(const QString &id) {
  cfgManager().mergeWithUser(
      {.launcher_window = config::Partial<config::WindowConfig>{.material = id.toStdString()}});
}

QVariantList GeneralSettingsModel::themeItems() const {
  QVariantList items;
  for (const auto &theme : ThemeService::instance().themes()) {
    auto iconUrl = theme->icon() ? ImageURL::local(QString::fromStdString(theme->icon()->string()))
                                 : ImageURL::builtin("vicinae");
    items.append(makeDropdownItem(theme->id(), theme->name(), qml::imageSourceFor(iconUrl)));
  }
  return wrapSection(QStringLiteral("Themes"), items);
}

QVariant GeneralSettingsModel::currentTheme() const {
  auto id = QString::fromStdString(cfg().systemTheme().name);
  auto *theme = ThemeService::instance().findTheme(id);
  if (!theme) return makeDropdownItem(id, id);
  auto iconUrl = theme->icon() ? ImageURL::local(QString::fromStdString(theme->icon()->string()))
                               : ImageURL::builtin("vicinae");
  return makeDropdownItem(id, theme->name(), qml::imageSourceFor(iconUrl));
}

QVariantList GeneralSettingsModel::fontItems() const {
  if (m_fontItems.isEmpty()) {
    QVariantList items;
    for (const auto &family : ServiceRegistry::instance()->fontService()->families()) {
      items.append(makeDropdownItem(family, family));
    }
    m_fontItems = wrapSection(QStringLiteral("Fonts"), items);
  }
  return m_fontItems;
}

QVariant GeneralSettingsModel::currentFont() const {
  auto &family = cfg().font.normal.family;
  auto name = family == "auto" ? QGuiApplication::font().family() : QString::fromStdString(family);
  return makeDropdownItem(name, name);
}

QVariantList GeneralSettingsModel::iconThemeItems() const {
  QVariantList items;
#ifdef Q_OS_LINUX
  IconThemeDatabase const db;
  for (const auto &theme : db.themes()) {
    items.append(makeDropdownItem(theme.name, theme.name));
  }
#endif
  return wrapSection(QStringLiteral("Icon Themes"), items);
}

QVariant GeneralSettingsModel::currentIconTheme() const {
  auto &ith = cfg().systemTheme().icon_theme;
  auto name = ith == "auto" ? QIcon::themeName() : QString::fromStdString(ith);
  return makeDropdownItem(name, name);
}

QVariantList GeneralSettingsModel::faviconServiceItems() const {
  QVariantList items;
  for (const auto &svc : FaviconService::providers()) {
    auto iconSource = qml::imageSourceFor(svc.icon);
    items.append(makeDropdownItem(svc.id, svc.name, iconSource));
  }
  return wrapSection(QStringLiteral("Favicon Services"), items);
}

QVariant GeneralSettingsModel::currentFaviconService() const {
  auto id = QString::fromStdString(cfg().favicon_service);
  for (const auto &svc : FaviconService::providers()) {
    if (svc.id == id) return makeDropdownItem(id, svc.name);
  }
  return makeDropdownItem(id, id);
}

QVariantList GeneralSettingsModel::keybindingSchemeItems() const {
  QVariantList items;
  items.append(makeDropdownItem(QStringLiteral("default"), QStringLiteral("Default")));
  items.append(makeDropdownItem(QStringLiteral("emacs"), QStringLiteral("Emacs")));
  return wrapSection(QStringLiteral("Keybinding Schemes"), items);
}

QVariant GeneralSettingsModel::currentKeybindingScheme() const {
  auto id = QString::fromStdString(cfg().keybinding);
  auto name = id == "emacs" ? QStringLiteral("Emacs") : QStringLiteral("Default");
  return makeDropdownItem(id, name);
}

void GeneralSettingsModel::selectTheme(const QString &id) {
  cfgManager().mergeThemeConfig({.name = id.toStdString()});
}

void GeneralSettingsModel::selectFont(const QString &id) {
  cfgManager().mergeWithUser(
      {.font = config::Partial<config::FontConfig>{.normal = {.family = id.toStdString()}}});
}

void GeneralSettingsModel::selectIconTheme(const QString &id) {
  cfgManager().mergeThemeConfig({.icon_theme = id.toStdString()});
}

void GeneralSettingsModel::selectFaviconService(const QString &id) {
  cfgManager().mergeWithUser({.favicon_service = id.toStdString()});
}

void GeneralSettingsModel::selectKeybindingScheme(const QString &id) {
  cfgManager().mergeWithUser({.keybinding = id.toStdString()});
}

QString GeneralSettingsModel::toggleShortcut() const {
  return QString::fromStdString(cfg().global_shortcuts.toggle.value_or(""));
}

void GeneralSettingsModel::setToggleShortcut(const QString &shortcut) {
  cfgManager().mergeWithUser(
      {.global_shortcuts = config::Partial<config::GlobalShortcuts>{.toggle = shortcut.toStdString()}});
}
