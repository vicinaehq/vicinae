#pragma once
#include "services/shortcut/shortcut-db.hpp"
#include "services/shortcut/shortcut.hpp"
#include <qobject.h>
#include <qtmetamacros.h>
#include <filesystem>
#include <memory>
#include <vector>

class OmniDatabase;

class ShortcutService : public QObject {
  Q_OBJECT

  ShortcutDatabase m_db;
  std::vector<std::shared_ptr<Shortcut>> m_shortcuts;

  void loadAll();
  void migrateFromDatabase(OmniDatabase &db);
  static std::shared_ptr<Shortcut> fromSerialized(const shortcut::SerializedShortcut &s);

public:
  const std::vector<std::shared_ptr<Shortcut>> &shortcuts() const;

  bool removeShortcut(const QString &id);
  bool createShortcut(const QString &name, const QString &icon, const QString &url, const QString &app);
  bool updateShortcut(const QString &id, const QString &name, const QString &icon, const QString &url,
                      const QString &app);
  Shortcut *findById(const QString &id);
  bool registerVisit(const QString &id);

  ShortcutService(const std::filesystem::path &path, OmniDatabase *db = nullptr);

signals:
  void shortcutSaved(const Shortcut &shortcut) const;
  void shortcutRemoved(const QString &id);
  void shortcutUpdated(const QString &id);
  void shortcutVisited(const QString &id);
};
