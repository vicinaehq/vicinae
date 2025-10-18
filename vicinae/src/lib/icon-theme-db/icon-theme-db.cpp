#include <filesystem>
#include <qicon.h>
#include <qlogging.h>
#include <qsettings.h>
#include <set>
#include "icon-theme-db/icon-theme-db.hpp"

namespace fs = std::filesystem;

static bool m_scanned = false;
static IconThemeDatabase::IconThemeList scan();
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

QString IconThemeDatabase::guessBestTheme() const {
  std::vector<QString> wellKnown = {"Adwaita Dark", "Adwaita", "Breeze", "Breeze Dark"};

  for (const auto &theme : wellKnown) {
    if (hasTheme(theme)) { return theme; }
  }

  if (auto it = std::ranges::find_if(m_themes, [&](auto &&info) { return !info.hidden; });
      it != m_themes.end()) {
    return it->name;
  }

  if (!m_themes.empty()) { return m_themes.front().name; }

  return "hicolor";
}

bool IconThemeDatabase::hasTheme(const QString &name) const {
  return std::ranges::any_of(m_themes, [&](auto &&info) { return info.name == name; });
}

bool IconThemeDatabase::isSuitableTheme(const QString &name) const { return hasTheme(name); }

IconThemeDatabase::IconThemeList IconThemeDatabase::scan() {
  IconThemeDatabase::IconThemeList themes;
  std::set<QString> seen;

  for (const auto &s : QIcon::themeSearchPaths()) {
    fs::path path = s.toStdString();
    std::error_code ec;

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (!entry.is_directory(ec)) continue;

      fs::path manifest = entry.path() / "index.theme";

      if (!fs::is_regular_file(manifest, ec)) {
        qWarning() << "Ignoring icon theme directory with no index.theme at" << manifest.c_str();
        continue;
      }

      // I guess the correct way would be to parse this with xdg desktop file
      // format instead of plain ini stuff, but for this use case this is fine.
      QSettings ini(manifest.c_str(), QSettings::IniFormat);
      IconThemeInfo info;

      ini.beginGroup("Icon Theme");
      info.name = ini.value("Name").toString();
      info.hidden = ini.value("Hidden").toString() == "true";
      ini.endGroup();

      if (seen.contains(info.name)) continue;

      seen.insert(info.name);
      themes.emplace_back(info);
    }
  }

  return themes;
}
