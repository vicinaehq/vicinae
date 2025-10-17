#include "theme/theme-db.hpp"
#include "theme/theme-file.hpp"
#include "xdgpp/env/env.hpp"
#include <filesystem>
#include <qfilesystemwatcher.h>
#include <qlogging.h>
#include <QApplication>

ThemeDatabase::ThemeDatabase() : m_watcher(new QFileSystemWatcher) {
  m_searchPaths = defaultSearchPaths();
  reinstallWatches();
  connect(m_watcher.get(), &QFileSystemWatcher::directoryChanged, this, &ThemeDatabase::directoryChanged);
}

void ThemeDatabase::scan() {
  std::error_code ec;

  std::vector<std::shared_ptr<ThemeFile>> themes;
  std::unordered_map<QString, std::shared_ptr<ThemeFile>> mapping;
  auto defaultDark = std::make_shared<ThemeFile>(ThemeFile::vicinaeDark());
  auto defaultLight = std::make_shared<ThemeFile>(ThemeFile::vicinaeLight());

  themes.emplace_back(defaultDark);
  mapping[defaultDark->id()] = defaultDark;
  themes.emplace_back(defaultLight);
  mapping[defaultLight->id()] = defaultLight;

  for (const auto &path : m_searchPaths) {
    for (const auto &entry : std::filesystem::directory_iterator(path, ec)) {
      if (entry.is_directory()) continue;
      if (entry.path().extension() != ".toml") continue;
      auto res = ThemeFile::fromFile(entry.path());

      if (!res) {
        qWarning() << "Failed to parse theme file at" << entry.path().c_str() << res.error();
        continue;
      }

      if (mapping.contains(res.value().id())) { continue; }

      auto file = std::make_shared<ThemeFile>(res.value());
      themes.emplace_back(file);
      mapping[file->id()] = file;
    }
  }

  for (const auto theme : m_themes) {
    if (theme->id() == defaultDark->id() || theme->id() == defaultLight->id()) continue;
    if (auto it = mapping.find(theme->inherits()); it != mapping.end()) {
      theme->setParent(it->second);
    } else {
      qWarning() << "failed to find inherited theme" << theme->inherits();
      theme->setParent(theme->variant() == ThemeVariant::Dark ? defaultDark : defaultLight);
    }
    emit themeChanged(*theme);
  }

  m_themes = themes;
}

ThemeDatabase::ThemeList ThemeDatabase::themes() const { return m_themes; }

const ThemeFile *ThemeDatabase::ThemeDatabase::theme(const QString &id) {
  auto pred = [&](const std::shared_ptr<ThemeFile> &file) { return file->id() == id; };
  if (auto it = std::ranges::find_if(m_themes, pred); it != m_themes.end()) return it->get();
  return nullptr;
}

std::vector<std::filesystem::path> ThemeDatabase::defaultSearchPaths() {
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
