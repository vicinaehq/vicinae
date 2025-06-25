#pragma once
#include "common.hpp"
#include "omni-database.hpp"
#include "root-item-manager.hpp"
#include <chrono>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qtmetamacros.h>

struct CommandDbEntry {
  std::shared_ptr<AbstractCmd> command;
  bool disabled;
  QString repositoryId;
  int openCount;
  std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastOpenedAt;
};

class OmniCommandDatabase : public QObject {
  Q_OBJECT

  OmniDatabase &db;
  std::vector<CommandDbEntry> entries;
  std::vector<std::shared_ptr<AbstractCommandRepository>> repositories;

  bool insertRepository(const QString &id) {
    QSqlQuery query(db.db());

    query.prepare("INSERT INTO extension (id) VALUES (:id) ON CONFLICT DO NOTHING;");
    query.bindValue(":id", id);

    if (!query.exec()) {
      qDebug() << "Failed to insertRepository" << query.lastError();
      return false;
    }

    return true;
  }

  const AbstractCommandRepository *findRepository(const QString &id) {
    for (const auto &repo : repositories) {
      if (repo->id() == id) return repo.get();
    }

    return nullptr;
  }

public:
  std::vector<CommandDbEntry> commands() const { return entries; }

  OmniCommandDatabase(OmniDatabase &db, RootItemManager &rootItemManager) : db(db) {
    QSqlQuery query(db.db());

    query.exec(R"(
		CREATE TABLE IF NOT EXISTS extension (
			id TEXT PRIMARY KEY,
			preference_values JSON DEFAULT '{}'
		);
	)");

    if (!query.exec(R"(
		CREATE TABLE IF NOT EXISTS command (
			id TEXT PRIMARY KEY,
			extension_id TEXT,
			preference_values JSON DEFAULT '{}',
			disabled INT DEFAULT 0,
			open_count INT DEFAULT 0,
			last_opened_at INT,
			FOREIGN KEY(extension_id) 
			REFERENCES extension(id)
			ON DELETE CASCADE
		);
	)")) {
      qDebug() << "Failed to create command table" << query.lastError();
    }
  }

  void resetCommandRanking(const QString &id) {
    auto query = db.createQuery();
    auto cmd = findCommand(id);

    if (!cmd) return;

    query.prepare(R"(
	  	UPDATE command 
		SET open_count = 0, last_opened_at = NULL,
		WHERE id = :id
		RETURNING open_count, last_opened_at
	  )");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
      qCritical() << "Failed to reset ranking for" << id << query.lastError();
      return;
    }

    cmd->openCount = 0;
    cmd->lastOpenedAt.reset();
  }

  void registerCommandOpen(const QString &id) {
    auto query = db.createQuery();
    auto cmd = findCommand(id);

    if (!cmd) return;

    query.prepare(R"(
	  	UPDATE command 
		SET open_count = open_count + 1, last_opened_at = unixepoch() 
		WHERE id = :id
		RETURNING open_count, last_opened_at
	  )");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
      qCritical() << "registerCommandOpen: failed to register" << id << query.lastError();
      return;
    }

    cmd->openCount = query.value(0).toInt();

    std::time_t epoch = query.value(1).toULongLong();

    cmd->lastOpenedAt = std::chrono::system_clock::from_time_t(epoch);
  }

  CommandDbEntry *findCommand(const QString &id) {
    for (auto &cmd : entries) {
      if (cmd.command->uniqueId() == id) return &cmd;
    }

    return nullptr;
  }

  const CommandDbEntry *findCommand(const QString &id) const {
    for (const auto &cmd : entries) {
      if (cmd.command->uniqueId() == id) return &cmd;
    }

    return nullptr;
  }

  bool hasCommand(const QString &id) const {
    for (const auto &cmd : entries) {
      if (cmd.command->uniqueId() == id) return true;
    }

    return false;
  }

  void registerCommand(const QString &repositoryId, const std::shared_ptr<AbstractCmd> &cmd) {
    QSqlQuery query(db.db());

    qDebug() << "registering command with id" << cmd->uniqueId();

    query.prepare(R"(
		INSERT INTO command (id, extension_id, disabled)
		VALUES (:id, :extension_id, :disabled)
		ON CONFLICT(id) DO UPDATE SET open_count = open_count, last_opened_at = last_opened_at
		RETURNING open_count, last_opened_at
	)");
    query.bindValue(":id", cmd->uniqueId());
    query.bindValue(":extension_id", repositoryId);
    query.bindValue(":disabled", false);

    if (!query.exec() || !query.next()) { qDebug() << "Failed to register command" << cmd->uniqueId(); }
    if (!hasCommand(cmd->uniqueId())) {
      CommandDbEntry entry;

      entry.command = cmd;
      entry.disabled = false;
      entry.repositoryId = repositoryId;
      entry.openCount = query.value(0).toInt();

      qDebug() << "command" << cmd->uniqueId() << "open count" << entry.openCount;

      auto lastOpenedAtField = query.value(1);

      if (!lastOpenedAtField.isNull()) {
        std::time_t epoch = lastOpenedAtField.toULongLong();

        entry.lastOpenedAt = std::chrono::system_clock::from_time_t(epoch);
      }

      // TODO: use something better
      entries.insert(entries.begin(), entry);
      emit commandRegistered(entry);
    }
  }

  void registerRepository(const std::shared_ptr<AbstractCommandRepository> &repository) {
    if (!db.db().transaction()) {
      qDebug() << "Failed to create transaction" << db.db().lastError();
      return;
    }

    if (!insertRepository(repository->id())) {
      qDebug() << "registerRepository: failed to insert repository";
      return;
    }

    for (const auto &cmd : repository->commands()) {
      registerCommand(repository->id(), cmd);
    }

    int index = -1;

    for (int i = 0; i != repositories.size(); ++i) {
      if (repositories[i]->id() == repository->id()) {
        repositories[i] = repository;
        index = i;
        break;
      }
    }

    if (index == -1) { repositories.push_back(repository); }

    db.db().commit();
    emit registryAdded(repository);
  }

signals:
  void commandRegistered(const CommandDbEntry &entry) const;
  void registryAdded(const std::shared_ptr<AbstractCommandRepository> &registry) const;
};
