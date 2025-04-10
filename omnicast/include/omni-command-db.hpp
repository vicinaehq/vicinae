#pragma once
#include "command-database.hpp"
#include "omni-database.hpp"
#include <qjsonobject.h>
#include <qlogging.h>
#include <qsqlquery.h>

struct CommandDbEntry {
  std::shared_ptr<AbstractCmd> command;
  bool disabled;
};

class OmniCommandDatabase {
  OmniDatabase &db;
  std::vector<CommandDbEntry> entries;

public:
  std::vector<CommandDbEntry> commands() const { return entries; }

  OmniCommandDatabase(OmniDatabase &db) : db(db) {
    QSqlQuery query(db.db());

    query.exec(R"(
		CREATE TABLE IF NOT EXISTS extension (
			id TEXT PRIMARY KEY,
			preference_values JSON
		);
	)");

    if (!query.exec(R"(
		CREATE TABLE IF NOT EXISTS command (
			id TEXT PRIMARY KEY,
			extension_id TEXT,
			preference_values JSON,
			disabled INT DEFAULT 0,
			activation_count INT DEFAULT 0,
			FOREIGN KEY(extension_id) 
			REFERENCES extension(id)
			ON DELETE CASCADE
		);
	)")) {
      qDebug() << "Failed to create command table" << query.lastError();
    }
  }

  bool hasCommand(const QString &id) const {
    for (const auto &cmd : entries) {
      if (cmd.command->id() == id) return true;
    }

    return false;
  }

  QJsonObject getPreferenceValues(const QString &id) const { return {}; }

  void setPreferenceValues(const QString &id, const QJsonObject &preferences) {}

  bool setDisable(const QString &id, bool value) {
    for (auto &cmd : entries) {
      if (cmd.command->id() == id) {
        qDebug() << "found command";
        QSqlQuery query(db.db());

        query.prepare("UPDATE command SET disabled = :disabled WHERE id = :id");
        query.bindValue(":disabled", value);
        query.bindValue(":id", id);

        if (query.exec()) {
          cmd.disabled = value;
          return true;
        } else {
          qDebug() << "failed to update visibility" << query.lastError();
          return false;
        }
      }
    }

    qDebug() << "command not found";
    return false;
  }

  void registerCommand(const std::shared_ptr<AbstractCmd> &cmd) {
    QSqlQuery query(db.db());

    qDebug() << "registering command with id" << cmd->id();

    query.prepare(R"(
		INSERT INTO command (id, disabled)
		VALUES (:id, :disabled)
		ON CONFLICT DO NOTHING
	)");
    query.bindValue(":id", cmd->id());
    query.bindValue(":disabled", false);

    if (!query.exec()) { qDebug() << "Failed to register command" << cmd->id(); }
    if (!hasCommand(cmd->id())) { entries.push_back(CommandDbEntry{.command = cmd, .disabled = false}); }
  }

  void registerRepository(const std::shared_ptr<AbstractCommandRepository> &repository) {
    if (!db.db().transaction()) {
      qDebug() << "Failed to create transaction" << db.db().lastError();
      return;
    }

    for (const auto &cmd : repository->commands()) {
      registerCommand(cmd);
    }

    db.db().commit();
  }
};
