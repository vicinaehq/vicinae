#include "general-settings-model.hpp"
#include "image-url.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "lib/icon-theme-db/icon-theme-db.hpp"
#include "services/keybinding/keybinding-service.hpp"
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

bool GeneralSettingsModel::searchFilesInRoot() const { return cfg().searchFilesInRoot; }
void GeneralSettingsModel::setSearchFilesInRoot(bool v) {
  cfgManager().mergeWithUser({.searchFilesInRoot = v});
}

bool GeneralSettingsModel::closeOnFocusLoss() const { return cfg().closeOnFocusLoss; }
void GeneralSettingsModel::setCloseOnFocusLoss(bool v) {
  cfgManager().mergeWithUser({.closeOnFocusLoss = v});
}

bool GeneralSettingsModel::considerPreedit() const { return cfg().considerPreedit; }
void GeneralSettingsModel::setConsiderPreedit(bool v) { cfgManager().mergeWithUser({.considerPreedit = v}); }

bool GeneralSettingsModel::popToRootOnClose() const { return cfg().popToRootOnClose; }
void GeneralSettingsModel::setPopToRootOnClose(bool v) {
  cfgManager().mergeWithUser({.popToRootOnClose = v});
}

bool GeneralSettingsModel::popOnBackspace() const { return cfg().popOnBackspace; }
void GeneralSettingsModel::setPopOnBackspace(bool v) { cfgManager().mergeWithUser({.popOnBackspace = v}); }

bool GeneralSettingsModel::clientSideDecorations() const {
  return cfg().launcherWindow.clientSideDecorations.enabled;
}
void GeneralSettingsModel::setClientSideDecorations(bool v) {
  cfgManager().mergeWithUser(
      {.launcherWindow = config::Partial<config::WindowConfig>{
           .clientSideDecorations = config::Partial<config::WindowCSD>{.enabled = v}}});
}

QString GeneralSettingsModel::windowOpacity() const { return QString::number(cfg().launcherWindow.opacity); }
void GeneralSettingsModel::setWindowOpacity(const QString &v) {
  bool ok = false;
  float val = v.toFloat(&ok);
  if (ok)
    cfgManager().mergeWithUser({.launcherWindow = config::Partial<config::WindowConfig>{.opacity = val}});
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
  QVariantList items;
  for (const auto &family : ServiceRegistry::instance()->fontService()->families()) {
    items.append(makeDropdownItem(family, family));
  }
  return wrapSection(QStringLiteral("Fonts"), items);
}

QVariant GeneralSettingsModel::currentFont() const {
  auto &family = cfg().font.normal.family;
  auto name = family == "auto" ? QGuiApplication::font().family() : QString::fromStdString(family);
  return makeDropdownItem(name, name);
}

QVariantList GeneralSettingsModel::iconThemeItems() const {
  QVariantList items;
  IconThemeDatabase const db;
  for (const auto &theme : db.themes()) {
    items.append(makeDropdownItem(theme.name, theme.name));
  }
  return wrapSection(QStringLiteral("Icon Themes"), items);
}

QVariant GeneralSettingsModel::currentIconTheme() const {
  auto &ith = cfg().systemTheme().iconTheme;
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
  auto id = QString::fromStdString(cfg().faviconService);
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
  cfgManager().mergeThemeConfig({.iconTheme = id.toStdString()});
}

void GeneralSettingsModel::selectFaviconService(const QString &id) {
  cfgManager().mergeWithUser({.faviconService = id.toStdString()});
}

void GeneralSettingsModel::selectKeybindingScheme(const QString &id) {
  cfgManager().mergeWithUser({.keybinding = id.toStdString()});
}
