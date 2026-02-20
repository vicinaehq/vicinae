#include "qml-general-settings-model.hpp"
#include "qml-image-url.hpp"
#include "qml-utils.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "lib/icon-theme-db/icon-theme-db.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include <QApplication>
#include <QIcon>

QmlGeneralSettingsModel::QmlGeneralSettingsModel(QObject *parent) : QObject(parent) {
  connect(ServiceRegistry::instance()->config(), &config::Manager::configChanged,
          this, &QmlGeneralSettingsModel::configChanged);
}

const config::ConfigValue &QmlGeneralSettingsModel::cfg() const {
  return ServiceRegistry::instance()->config()->value();
}

config::Manager &QmlGeneralSettingsModel::cfgManager() const {
  return *ServiceRegistry::instance()->config();
}

bool QmlGeneralSettingsModel::searchFilesInRoot() const { return cfg().searchFilesInRoot; }
void QmlGeneralSettingsModel::setSearchFilesInRoot(bool v) { cfgManager().mergeWithUser({.searchFilesInRoot = v}); }

bool QmlGeneralSettingsModel::closeOnFocusLoss() const { return cfg().closeOnFocusLoss; }
void QmlGeneralSettingsModel::setCloseOnFocusLoss(bool v) { cfgManager().mergeWithUser({.closeOnFocusLoss = v}); }

bool QmlGeneralSettingsModel::considerPreedit() const { return cfg().considerPreedit; }
void QmlGeneralSettingsModel::setConsiderPreedit(bool v) { cfgManager().mergeWithUser({.considerPreedit = v}); }

bool QmlGeneralSettingsModel::popToRootOnClose() const { return cfg().popToRootOnClose; }
void QmlGeneralSettingsModel::setPopToRootOnClose(bool v) { cfgManager().mergeWithUser({.popToRootOnClose = v}); }

bool QmlGeneralSettingsModel::clientSideDecorations() const { return cfg().launcherWindow.clientSideDecorations.enabled; }
void QmlGeneralSettingsModel::setClientSideDecorations(bool v) {
  cfgManager().mergeWithUser({.launcherWindow = config::Partial<config::WindowConfig>{
                                  .clientSideDecorations = config::Partial<config::WindowCSD>{.enabled = v}}});
}

QString QmlGeneralSettingsModel::windowOpacity() const { return QString::number(cfg().launcherWindow.opacity); }
void QmlGeneralSettingsModel::setWindowOpacity(const QString &v) {
  bool ok = false;
  float val = v.toFloat(&ok);
  if (ok) cfgManager().mergeWithUser({.launcherWindow = config::Partial<config::WindowConfig>{.opacity = val}});
}

QString QmlGeneralSettingsModel::fontSize() const { return QString::number(cfg().font.normal.size); }
void QmlGeneralSettingsModel::setFontSize(const QString &v) {
  bool ok = false;
  float val = v.toFloat(&ok);
  if (ok) cfgManager().mergeWithUser({.font = config::Partial<config::FontConfig>{.normal{.size = val}}});
}

static QVariantMap makeDropdownItem(const QString &id, const QString &displayName, const QString &iconSource = {}) {
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

QVariantList QmlGeneralSettingsModel::themeItems() const {
  QVariantList items;
  for (const auto &theme : ThemeService::instance().themes()) {
    auto iconUrl = theme->icon()
                       ? ImageURL::local(QString::fromStdString(theme->icon()->string()))
                       : ImageURL::builtin("vicinae");
    items.append(makeDropdownItem(theme->name(), theme->name(), qml::imageSourceFor(iconUrl)));
  }
  return wrapSection(QStringLiteral("Themes"), items);
}

QVariant QmlGeneralSettingsModel::currentTheme() const {
  auto name = QString::fromStdString(cfg().systemTheme().name);
  return makeDropdownItem(name, name);
}

QVariantList QmlGeneralSettingsModel::fontItems() const {
  QVariantList items;
  for (const auto &family : ServiceRegistry::instance()->fontService()->families()) {
    items.append(makeDropdownItem(family, family));
  }
  return wrapSection(QStringLiteral("Fonts"), items);
}

QVariant QmlGeneralSettingsModel::currentFont() const {
  auto &family = cfg().font.normal.family;
  auto name = family == "auto" ? QApplication::font().family() : QString::fromStdString(family);
  return makeDropdownItem(name, name);
}

QVariantList QmlGeneralSettingsModel::iconThemeItems() const {
  QVariantList items;
  IconThemeDatabase db;
  for (const auto &theme : db.themes()) {
    items.append(makeDropdownItem(theme.name, theme.name));
  }
  return wrapSection(QStringLiteral("Icon Themes"), items);
}

QVariant QmlGeneralSettingsModel::currentIconTheme() const {
  auto &ith = cfg().systemTheme().iconTheme;
  auto name = ith == "auto" ? QIcon::themeName() : QString::fromStdString(ith);
  return makeDropdownItem(name, name);
}

QVariantList QmlGeneralSettingsModel::faviconServiceItems() const {
  QVariantList items;
  for (const auto &svc : FaviconService::providers()) {
    auto iconSource = qml::imageSourceFor(svc.icon);
    items.append(makeDropdownItem(svc.id, svc.name, iconSource));
  }
  return wrapSection(QStringLiteral("Favicon Services"), items);
}

QVariant QmlGeneralSettingsModel::currentFaviconService() const {
  auto id = QString::fromStdString(cfg().faviconService);
  for (const auto &svc : FaviconService::providers()) {
    if (svc.id == id) return makeDropdownItem(id, svc.name);
  }
  return makeDropdownItem(id, id);
}

QVariantList QmlGeneralSettingsModel::keybindingSchemeItems() const {
  QVariantList items;
  items.append(makeDropdownItem(QStringLiteral("default"), QStringLiteral("Default")));
  items.append(makeDropdownItem(QStringLiteral("emacs"), QStringLiteral("Emacs")));
  return wrapSection(QStringLiteral("Keybinding Schemes"), items);
}

QVariant QmlGeneralSettingsModel::currentKeybindingScheme() const {
  auto id = QString::fromStdString(cfg().keybinding);
  auto name = id == "emacs" ? QStringLiteral("Emacs") : QStringLiteral("Default");
  return makeDropdownItem(id, name);
}

void QmlGeneralSettingsModel::selectTheme(const QString &id) {
  cfgManager().mergeThemeConfig({.name = id.toStdString()});
}

void QmlGeneralSettingsModel::selectFont(const QString &id) {
  cfgManager().mergeWithUser({.font = config::Partial<config::FontConfig>{.normal = {.family = id.toStdString()}}});
}

void QmlGeneralSettingsModel::selectIconTheme(const QString &id) {
  cfgManager().mergeThemeConfig({.iconTheme = id.toStdString()});
}

void QmlGeneralSettingsModel::selectFaviconService(const QString &id) {
  cfgManager().mergeWithUser({.faviconService = id.toStdString()});
}

void QmlGeneralSettingsModel::selectKeybindingScheme(const QString &id) {
  cfgManager().mergeWithUser({.keybinding = id.toStdString()});
}
