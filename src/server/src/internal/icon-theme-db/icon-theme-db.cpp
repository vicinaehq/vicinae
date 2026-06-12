#include <filesystem>
#include <qdir.h>
#include <qicon.h>
#include <qlogging.h>
#include <qprocess.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <set>
#include "icon-theme-db/icon-theme-db.hpp"
#include "utils/environment.hpp"

namespace fs = std::filesystem;

static bool m_scanned = false;
static IconThemeDatabase::IconThemeList m_themes;

IconThemeDatabase::IconThemeDatabase() {
  if (!m_scanned) {
    m_themes = scan();
    m_scanned = true;
  }
}

IconThemeDatabase::IconThemeList IconThemeDatabase::themes(bool includeHidden) const {
  if (includeHidden) return m_themes;

  IconThemeDatabase::IconThemeList themeList;

  themeList.reserve(m_themes.size());

  for (const auto &theme : m_themes) {
    if (!theme.hidden) { themeList.emplace_back(theme); }
  }

  return themeList;
}

std::optional<QString> IconThemeDatabase::systemIconThemeId() const {
#ifdef Q_OS_LINUX
  auto validated = [this](QString id) -> std::optional<QString> {
    id = id.trimmed();
    if (id.isEmpty()) return std::nullopt;
    if (hasThemeId(id)) return id;
    return std::nullopt;
  };

  if (Environment::isPlasmaDesktop()) {
    const QString path =
        QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)).filePath("kdeglobals");
    QSettings kde(path, QSettings::IniFormat);
    if (auto id = validated(kde.value("Icons/Theme").toString())) return id;
  }

  // gsettings is the source of truth on GNOME and most GTK-based desktops.
  QProcess gsettings;
  gsettings.start("gsettings", {"get", "org.gnome.desktop.interface", "icon-theme"});
  if (gsettings.waitForFinished(1000) && gsettings.exitStatus() == QProcess::NormalExit &&
      gsettings.exitCode() == 0) {
    QString out = QString::fromUtf8(gsettings.readAllStandardOutput()).trimmed();
    // gsettings wraps strings in single quotes: 'kora'
    if (out.startsWith('\'') && out.endsWith('\'') && out.size() >= 2) { out = out.mid(1, out.size() - 2); }
    if (auto id = validated(out)) return id;
  }

  // Fallback: GTK settings files (some setups write these even without gsettings access).
  for (const char *sub : {"gtk-4.0/settings.ini", "gtk-3.0/settings.ini"}) {
    const QString path =
        QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)).filePath(sub);
    if (!QFile::exists(path)) continue;
    QSettings gtk(path, QSettings::IniFormat);
    if (auto id = validated(gtk.value("Settings/gtk-icon-theme-name").toString())) return id;
  }
#endif

  return std::nullopt;
}

QString IconThemeDatabase::guessBestTheme() const {
  if (auto sys = systemIconThemeId()) { return *sys; }

  std::vector<QString> const wellKnown = {"Adwaita", "breeze", "Breeze", "oxygen"};

  for (const auto &theme : wellKnown) {
    if (hasThemeId(theme)) { return theme; }
  }

  if (auto it = std::ranges::find_if(m_themes, [&](auto &&info) { return !info.hidden; });
      it != m_themes.end()) {
    return it->id;
  }

  if (!m_themes.empty()) { return m_themes.front().id; }

  return "hicolor";
}

bool IconThemeDatabase::hasThemeId(const QString &id) const {
  return std::ranges::any_of(m_themes, [&](auto &&info) { return info.id == id; });
}

bool IconThemeDatabase::isSuitableTheme(const QString &id) const { return hasThemeId(id); }

QString IconThemeDatabase::resolveThemeId(const QString &value) const {
  if (hasThemeId(value)) return value;

  // Legacy configs (and older builds) stored the display name; map it back to the directory id.
  if (auto it = std::ranges::find_if(m_themes, [&](auto &&info) { return info.name == value; });
      it != m_themes.end()) {
    return it->id;
  }

  return value;
}

QString IconThemeDatabase::displayName(const QString &id) const {
  if (auto it = std::ranges::find_if(m_themes, [&](auto &&info) { return info.id == id; });
      it != m_themes.end()) {
    return it->name;
  }
  return id;
}

IconThemeDatabase::IconThemeList IconThemeDatabase::scan() {
  IconThemeDatabase::IconThemeList themes;
  std::set<QString> seen;

  for (const auto &s : QIcon::themeSearchPaths()) {
    fs::path const path = s.toStdString();
    std::error_code ec;

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (!entry.is_directory(ec)) continue;

      fs::path const manifest = entry.path() / "index.theme";

      if (!fs::is_regular_file(manifest, ec)) {
        qWarning() << "Ignoring icon theme directory with no index.theme at" << manifest.c_str();
        continue;
      }

      // I guess the correct way would be to parse this with xdg desktop file
      // format instead of plain ini stuff, but for this use case this is fine.
      QSettings ini(manifest.c_str(), QSettings::IniFormat);
      IconThemeInfo info;

      info.id = QString::fromStdString(entry.path().filename().string());

      ini.beginGroup("Icon Theme");
      info.name = ini.value("Name").toString();
      info.hidden = ini.value("Hidden").toString() == "true";
      ini.endGroup();

      if (info.name.isEmpty()) info.name = info.id;

      // The directory id is the canonical identifier; the same theme may appear under several search
      // paths (system + user), so dedup on it rather than the display name.
      if (seen.contains(info.id)) continue;

      seen.insert(info.id);
      themes.emplace_back(info);
    }
  }

  return themes;
}
