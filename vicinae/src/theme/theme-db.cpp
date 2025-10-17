#include "theme/theme-db.hpp"
#include "theme/theme-file.hpp"
#include "xdgpp/env/env.hpp"
#include <qfilesystemwatcher.h>
#include <qlogging.h>
#include <QApplication>

ThemeDatabase::ThemeDatabase() : m_watcher(new QFileSystemWatcher) {
  m_searchPaths = defaultSearchPaths();
  reinstallWatches();
  connect(m_watcher.get(), &QFileSystemWatcher::directoryChanged, this, &ThemeDatabase::directoryChanged);
}

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
      auto themeFile = std::make_shared<ThemeFile>(res.value());

      m_themes.insert({themeFile->id(), themeFile});
    }
  }

  for (const auto &[k, v] : m_themes) {
    if (k == "vicinae-dark" || k == "vicinae-light") continue;

    if (auto it = m_themes.find(v->inherits()); it != m_themes.end()) {
      v->setParent(it->second);
    } else {
      qWarning() << "failed to find inherited theme" << v->inherits();
    }
    emit themeChanged(*v);
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

void ThemeDatabase::scanPath(const std::filesystem::path &path) {}

void ThemeDatabase::directoryChanged(const QString &path) {
  qDebug() << "theme directory changed" << path;
  scan();
}

void ThemeDatabase::reinstallWatches() {
  for (const auto &path : m_watcher->directories()) {
    m_watcher->removePath(path);
  }
  for (const auto &path : searchPaths()) {
    m_watcher->addPath(path.c_str());
  }
}
