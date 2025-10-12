#pragma once
#include "theme-file.hpp"
#include "xdgpp/env/env.hpp"
#include <filesystem>
#include <QDebug>
#include <unordered_map>

class ThemeDatabase {
public:
  using ThemeMap = std::unordered_map<QString, ThemeFile>;

  ThemeDatabase() { m_searchPaths = defaultSearchPaths(); }

  std::vector<std::filesystem::path> searchPaths() const { return m_searchPaths; }
  void setSearchPaths(const std::vector<std::filesystem::path> &paths) { m_searchPaths = paths; }

  void scan() {
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
        m_themes.insert({entry.path().filename().c_str(), res.value()});
      }
    }
  }

  const ThemeFile *theme(const QString &id) {
    if (auto it = m_themes.find(id); it != m_themes.end()) return &it->second;
    return nullptr;
  }

  const ThemeMap &themes() { return m_themes; }

private:
  std::vector<std::filesystem::path> defaultSearchPaths() const {
    std::vector<std::filesystem::path> paths;
    auto dd = xdgpp::dataDirs();
    paths.reserve(dd.size());
    for (const auto &dir : dd) {
      paths.emplace_back(dir / "vicinae" / "themes");
    }
    return paths;
  }

  std::vector<std::filesystem::path> m_searchPaths;
  ThemeMap m_themes;
};
