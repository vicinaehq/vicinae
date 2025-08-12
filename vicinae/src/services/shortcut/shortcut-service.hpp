#pragma once
#include "services/shortcut/shortcut.hpp"
#include "omni-database.hpp"
#include <qlogging.h>
#include <qdebug.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qstring.h>
#include <qsqlerror.h>
#include <qtmetamacros.h>

class ShortcutService : public QObject {
  Q_OBJECT

  OmniDatabase &m_db;
  std::vector<std::shared_ptr<Shortcut>> m_shortcuts;
  std::vector<std::shared_ptr<Shortcut>> loadAll();

public:
  std::vector<std::shared_ptr<Shortcut>> shortcuts() const;

  bool removeShortcut(const QString &id);
  bool createShortcut(const QString &name, const QString &icon, const QString &url, const QString &app);
  bool updateShortcut(const QString &id, const QString &name, const QString &icon, const QString &url,
                      const QString &app);
  Shortcut *findById(const QString &id);
  bool registerVisit(const QString &id);

  ShortcutService(OmniDatabase &db);

signals:
  void shortcutSaved(const Shortcut &shortcut) const;
  void shortcutRemoved(const QString &id);
  void shortcutUpdated(const QString &id);
  void shortcutVisited(const QString &id);
};
