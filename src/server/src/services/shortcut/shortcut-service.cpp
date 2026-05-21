#include "services/shortcut/shortcut-service.hpp"
#include "omni-database.hpp"
#include <qlogging.h>

std::shared_ptr<Shortcut> ShortcutService::fromSerialized(const shortcut::SerializedShortcut &s) {
  auto sc = std::make_shared<Shortcut>();
  sc->setId(QString::fromStdString(s.id));
  sc->setName(QString::fromStdString(s.name));
  sc->setIcon(QString::fromStdString(s.icon));
  sc->parseLink(QString::fromStdString(s.url));
  sc->setApp(QString::fromStdString(s.app));
  sc->setOpenCount(s.openCount);
  sc->setCreatedAt(QDateTime::fromSecsSinceEpoch(s.createdAt));
  sc->setUpdatedAt(QDateTime::fromSecsSinceEpoch(s.updatedAt));
  if (s.lastUsedAt) { sc->setLastOpenedAt(QDateTime::fromSecsSinceEpoch(*s.lastUsedAt)); }
  return sc;
}

void ShortcutService::loadAll() {
  m_shortcuts.clear();
  m_shortcuts.reserve(m_db.shortcuts().size());
  for (const auto &s : m_db.shortcuts()) {
    m_shortcuts.emplace_back(fromSerialized(s));
  }
}

void ShortcutService::migrateFromDatabase(OmniDatabase &db) {
  auto checkStmt = db.db().prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='shortcut'");

  if (!checkStmt.step()) return;

  auto stmt = db.db().prepare(R"(
    SELECT id, name, icon, url, app, open_count, created_at, updated_at, last_used_at
    FROM shortcut
  )");

  std::vector<shortcut::SerializedShortcut> migrated;

  while (stmt.step()) {
    shortcut::SerializedShortcut s;
    s.id = stmt.columnText(0);
    s.name = stmt.columnText(1);
    s.icon = stmt.columnText(2);
    s.url = stmt.columnText(3);
    s.app = stmt.columnText(4);
    s.openCount = stmt.columnInt(5);
    s.createdAt = stmt.columnUInt64(6);
    s.updatedAt = stmt.columnUInt64(7);
    if (!stmt.isNull(8)) { s.lastUsedAt = stmt.columnUInt64(8); }
    migrated.emplace_back(std::move(s));
  }

  if (migrated.empty()) return;

  if (const auto result = m_db.setShortcuts(migrated); !result) {
    qCritical() << "Failed to migrate shortcuts to JSON:" << result.error().c_str();
    return;
  }

  m_db.reload();
  qInfo() << "Migrated" << migrated.size() << "shortcuts from database to JSON file";
}

Shortcut *ShortcutService::findById(const QString &id) {
  if (auto it = std::ranges::find_if(m_shortcuts, [&](const auto &bk) { return id == bk->id(); });
      it != m_shortcuts.end()) {
    return it->get();
  }

  return nullptr;
}

bool ShortcutService::registerVisit(const QString &id) {
  auto stdId = id.toStdString();

  if (const auto result = m_db.registerVisit(stdId); !result) {
    qWarning() << "Failed to register shortcut visit:" << result.error().c_str();
    return false;
  }

  auto *shortcut = findById(id);
  auto *s = m_db.findById(stdId);
  if (!shortcut || !s) return false;

  shortcut->setOpenCount(s->openCount);
  if (s->lastUsedAt) { shortcut->setLastOpenedAt(QDateTime::fromSecsSinceEpoch(*s->lastUsedAt)); }

  emit shortcutVisited(id);
  return true;
}

const std::vector<std::shared_ptr<Shortcut>> &ShortcutService::shortcuts() const { return m_shortcuts; }

bool ShortcutService::updateShortcut(const QString &id, const QString &name, const QString &icon,
                                     const QString &url, const QString &app) {
  if (const auto result = m_db.updateShortcut(id.toStdString(), name.toStdString(), icon.toStdString(),
                                              url.toStdString(), app.toStdString());
      !result) {
    qCritical() << "Failed to update shortcut:" << result.error().c_str();
    return false;
  }

  auto *shortcut = findById(id);
  if (!shortcut) return false;

  shortcut->setName(name);
  shortcut->setIcon(icon);
  shortcut->parseLink(url);
  shortcut->setApp(app);

  emit shortcutUpdated(id);
  return true;
}

bool ShortcutService::removeShortcut(const QString &id) {
  if (const auto result = m_db.removeShortcut(id.toStdString()); !result) {
    qCritical() << "Failed to remove shortcut:" << result.error().c_str();
    return false;
  }

  auto it = std::ranges::remove_if(m_shortcuts, [&](const auto &mark) { return mark->id() == id; });
  m_shortcuts.erase(it.begin(), it.end());

  emit shortcutRemoved(id);
  return true;
}

bool ShortcutService::createShortcut(const QString &name, const QString &icon, const QString &url,
                                     const QString &app) {
  auto result =
      m_db.addShortcut(name.toStdString(), icon.toStdString(), url.toStdString(), app.toStdString());

  if (!result) {
    qWarning() << "Failed to save shortcut:" << result.error().c_str();
    return false;
  }

  auto shortcut = fromSerialized(*result);
  m_shortcuts.emplace_back(shortcut);

  emit shortcutSaved(*shortcut);
  return true;
}

ShortcutService::ShortcutService(const std::filesystem::path &path, OmniDatabase *db) : m_db(path) {
  if (db && m_db.shortcuts().empty()) { migrateFromDatabase(*db); }
  loadAll();
}
