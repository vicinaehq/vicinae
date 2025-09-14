#include <filesystem>
#include <qicon.h>
#include <qsettings.h>
#include <system_error>
#include <ranges>
#include "icon-theme-db/icon-theme-db.hpp"

namespace fs = std::filesystem;

static bool m_scanned;
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

  auto isVisible = [](auto &&info) { return !info.hidden; };

  return m_themes | std::views::filter(isVisible) | std::ranges::to<std::vector>();
}

bool IconThemeDatabase::hasTheme(const QString &name) const {
  return std::ranges::any_of(m_themes, [&](auto &&info) { return info.name == name; });
}

bool IconThemeDatabase::isSuitableTheme(const QString &name) const {
  auto isTheme = [&](auto &&info) { return info.name == name; };

  if (auto it = std::ranges::find_if(m_themes, isTheme); it != m_themes.end()) { return !it->hidden; }

  return false;
}

IconThemeDatabase::IconThemeList IconThemeDatabase::scan() {
  IconThemeDatabase::IconThemeList themes;
  std::set<QString> seen;

  for (const auto &s : QIcon::themeSearchPaths()) {
    fs::path path = s.toStdString();
    std::error_code ec;

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (!entry.is_directory(ec)) continue;

      fs::path manifest = entry.path() / "index.theme";

      if (!fs::is_regular_file(manifest, ec)) continue;

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
