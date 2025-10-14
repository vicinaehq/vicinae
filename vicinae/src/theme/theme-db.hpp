#pragma once
#include "theme-file.hpp"
#include <filesystem>
#include <QDebug>
#include <unordered_map>

class ThemeDatabase {
public:
  using ThemeMap = std::unordered_map<QString, std::shared_ptr<ThemeFile>>;
  using ThemeList = std::vector<std::shared_ptr<ThemeFile>>;

  ThemeDatabase() { m_searchPaths = defaultSearchPaths(); }

  std::vector<std::filesystem::path> searchPaths() const { return m_searchPaths; }
  void setSearchPaths(const std::vector<std::filesystem::path> &paths) { m_searchPaths = paths; }
  void scan();

  const ThemeFile *theme(const QString &id);
  ThemeList themes() const;

private:
  std::vector<std::filesystem::path> defaultSearchPaths() const;
  std::vector<std::filesystem::path> m_searchPaths;
  ThemeMap m_themes;
};
