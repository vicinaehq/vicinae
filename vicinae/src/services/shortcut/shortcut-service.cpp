#include "services/shortcut/shortcut-service.hpp"
#include "shortcut-service.hpp"
#include "crypto.hpp"
#include <qlogging.h>

std::vector<std::shared_ptr<Shortcut>> ShortcutService::loadAll() {
  std::vector<std::shared_ptr<Shortcut>> shortcuts;
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		SELECT id, name, icon, url, app, open_count, created_at, updated_at, last_used_at
		FROM shortcut
	)");

  if (!query.exec()) {
    qCritical() << "Failed to execute loadAll query: " << query.lastError();
    return {};
  }

  while (query.next()) {
    Shortcut shortcut;

    shortcut.setId(query.value(0).toString());
    shortcut.setName(query.value(1).toString());
    shortcut.setIcon(query.value(2).toString());
    shortcut.parseLink(query.value(3).toString());
    shortcut.setApp(query.value(4).toString());
    shortcut.setOpenCount(query.value(5).toInt());
    shortcut.setCreatedAt(QDateTime::fromSecsSinceEpoch(query.value(6).toULongLong()));
    shortcut.setUpdatedAt(QDateTime::fromSecsSinceEpoch(query.value(7).toULongLong()));

    if (auto value = query.value(8); !value.isNull()) {
      shortcut.setLastOpenedAt(QDateTime::fromSecsSinceEpoch(value.toULongLong()));
    }

    shortcuts.emplace_back(std::make_shared<Shortcut>(shortcut));
  }

  return shortcuts;
}

Shortcut *ShortcutService::findById(const QString &id) {
  if (auto it = std::ranges::find_if(m_shortcuts, [&](auto bk) { return id == bk->id(); });
      it != m_shortcuts.end()) {
    return it->get();
  }

  return nullptr;
}

bool ShortcutService::registerVisit(const QString &id) {
  {
    QSqlQuery query = m_db.createQuery();

    query.prepare("UPDATE shortcut SET last_used_at = :epoch, open_count = open_count + 1 WHERE id = :id "
                  "RETURNING last_used_at, open_count");
    query.bindValue(":id", id);
    query.bindValue(":epoch", QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
      qWarning() << "Failed to register shortcut visit" << query.lastError();
      return false;
    }

    if (!query.next()) {
      qWarning() << "No shortcut" << id << "to update";
      return false;
    }

    auto shortcut = findById(id);

    if (!shortcut) {
      qWarning() << "no in memory shortcut with id" << id;
      return false;
    }

    auto lastOpenedAtEpoch = query.value(0).toULongLong();
    int count = query.value(1).toInt();

    shortcut->setLastOpenedAt(QDateTime::fromSecsSinceEpoch(lastOpenedAtEpoch));
    shortcut->setOpenCount(count);
  }

  emit shortcutVisited(id);

  return true;
}

std::vector<std::shared_ptr<Shortcut>> ShortcutService::shortcuts() const { return m_shortcuts; }

bool ShortcutService::updateShortcut(const QString &id, const QString &name, const QString &icon,
                                     const QString &url, const QString &app) {
  {
    auto it = std::ranges::find_if(m_shortcuts, [&](auto &&bk) { return bk->id() == id; });

    if (it == m_shortcuts.end()) {
      qCritical() << "No shortcut with id" << id;
      return false;
    }

    auto &shortcut = *it;
    QSqlQuery query = m_db.createQuery();

    query.prepare(R"(
		UPDATE shortcut
		SET name = :name, icon = :icon, url = :url, app = :app, updated_at = :updated_at
		WHERE id = :id
	)");
    query.bindValue(":name", name);
    query.bindValue(":icon", icon);
    query.bindValue(":url", url);
    query.bindValue(":app", app);
    query.bindValue(":id", id);
    query.bindValue(":updated_at", QDateTime::currentSecsSinceEpoch());

    if (!query.exec()) {
      qCritical() << "Failed to update shortcut" << query.lastError();
      return false;
    }

    shortcut->setName(name);
    shortcut->setIcon(icon);
    shortcut->parseLink(url);
    shortcut->setApp(app);
  }

  emit shortcutUpdated(id);

  return true;
}

bool ShortcutService::removeShortcut(const QString &id) {
  {
    QSqlQuery query = m_db.createQuery();

    query.prepare("DELETE FROM shortcut WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
      qCritical() << "Failed to remove shortcut" << query.lastError();
      return false;
    }

    auto it = std::ranges::remove_if(m_shortcuts, [id](const auto &mark) { return mark->id() == id; });

    m_shortcuts.erase(it.begin(), it.end());
  }

  emit shortcutRemoved(id);

  return true;
}

bool ShortcutService::createShortcut(const QString &name, const QString &icon, const QString &url,
                                     const QString &app) {
  Shortcut shortcut;

  {
    QSqlQuery query = m_db.createQuery();
    QString id = Crypto::UUID::v4();
    auto now = QDateTime::currentDateTime();

    query.prepare(R"(
		INSERT INTO shortcut (id, name, icon, url, app, created_at, updated_at) VALUES (:id, :name, :icon, :url, :app, :epoch, :epoch)
		RETURNING id, name, icon, url, app, open_count
  )");
    query.bindValue(":id", id);
    query.bindValue(":name", name);
    query.bindValue(":icon", icon);
    query.bindValue(":url", url);
    query.bindValue(":app", app);
    query.bindValue(":epoch", now.currentSecsSinceEpoch());

    if (!query.exec()) {
      qWarning() << "Failed to save shortcut" << query.lastError();
      return false;
    }

    if (!query.next()) {
      qWarning() << "no next";
      return false;
    }

    shortcut.setId(query.value(0).toString());
    shortcut.setName(query.value(1).toString());
    shortcut.setIcon(query.value(2).toString());
    shortcut.parseLink(query.value(3).toString());
    shortcut.setApp(query.value(4).toString());
    shortcut.setOpenCount(query.value(5).toInt());
    shortcut.setCreatedAt(now);
    shortcut.setUpdatedAt(now);
    m_shortcuts.emplace_back(std::make_shared<Shortcut>(shortcut));
  }

  emit shortcutSaved(shortcut);

  return true;
}

ShortcutService::ShortcutService(OmniDatabase &db) : m_db(db) { m_shortcuts = loadAll(); }
