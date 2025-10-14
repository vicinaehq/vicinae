#include "theme/theme-db.hpp"
#include "theme/theme-file.hpp"
#include "xdgpp/env/env.hpp"

void ThemeDatabase::scan() {
  m_themes.clear();
  m_themes.insert({"vicinae-dark", std::make_shared<ThemeFile>(ThemeFile::vicinaeDark())});
  m_themes.insert({"vicinae-light", std::make_shared<ThemeFile>(ThemeFile::vicinaeLight())});

  std::error_code ec;
  for (const auto &path : m_searchPaths) {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(path, ec)) {
      if (entry.is_directory()) continue;
      if (entry.path().extension() != ".toml") continue;
      auto res = ThemeFile::fromFile(entry.path());

      if (!res) {
        qWarning() << "Failed to parse theme file at" << entry.path().c_str() << res.error();
        continue;
      }
      QString id = entry.path().filename().c_str();

      m_themes.insert({id, std::make_shared<ThemeFile>(res.value())});
    }
  }
}

ThemeDatabase::ThemeList ThemeDatabase::themes() const {
  ThemeList list;
  list.reserve(m_themes.size());
  for (const auto &[k, v] : m_themes) {
    list.emplace_back(v);
  }
  return list;
}

const ThemeFile *ThemeDatabase::ThemeDatabase::theme(const QString &id) {
  if (auto it = m_themes.find(id); it != m_themes.end()) return it->second.get();
  return nullptr;
}

std::vector<std::filesystem::path> ThemeDatabase::defaultSearchPaths() const {
  std::vector<std::filesystem::path> paths;
  auto dd = xdgpp::dataDirs();
  paths.reserve(dd.size());
  for (const auto &dir : dd) {
    paths.emplace_back(dir / "vicinae" / "themes");
  }
  return paths;
}
