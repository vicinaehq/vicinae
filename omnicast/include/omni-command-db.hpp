#pragma once
#include "command-database.hpp"
#include "omni-database.hpp"
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

  OmniCommandDatabase(OmniDatabase &db) : db(db) {
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
      if (cmd.command->id() == id) return &cmd;
    }

    return nullptr;
  }

  const CommandDbEntry *findCommand(const QString &id) const {
    for (const auto &cmd : entries) {
      if (cmd.command->id() == id) return &cmd;
    }

    return nullptr;
  }

  bool hasCommand(const QString &id) const {
    for (const auto &cmd : entries) {
      if (cmd.command->id() == id) return true;
    }

    return false;
  }

  QJsonObject getPreferenceValues(const QString &id) const {
    auto cmdEntry = findCommand(id);

    if (!cmdEntry) {
      qDebug() << "No command entry with ID" << id;
      return {};
    };

    QSqlQuery query(db.db());

    query.prepare(R"(
		SELECT 
			json_patch(ext.preference_values, cmd.preference_values) as preference_values 
		FROM 
			command cmd 
		LEFT JOIN
			extension ext 
		ON 
			ext.id = cmd.extension_id
		WHERE
			cmd.id = :id
	)");
    query.addBindValue(id);

    if (!query.exec()) {
      qDebug() << "Failed to get preference values for command with ID" << id << query.lastError();
      return {};
    }

    if (!query.next()) {
      qDebug() << "No results";
      return {};
    }
    auto rawJson = query.value(0).toString();

    qDebug() << "raw preferences json" << rawJson;

    auto json = QJsonDocument::fromJson(rawJson.toUtf8());
    auto preferenceValues = json.object();

    for (auto pref : cmdEntry->command->preferences()) {
      auto dflt = pref->defaultValueAsJson();

      if (!preferenceValues.contains(pref->name()) && !dflt.isNull()) {
        preferenceValues[pref->name()] = dflt;
      }
    }

    return preferenceValues;
  }

  bool setRepositoryPreferenceValues(const QString &repositoryId, const QJsonObject &preferences) {
    QSqlQuery query(db.db());
    QJsonDocument json;

    json.setObject(preferences);
    query.prepare("UPDATE extension SET preference_values = :preferences WHERE id = :id");
    query.bindValue(":preferences", json.toJson());
    query.bindValue(":id", repositoryId);

    if (!query.exec()) {
      qDebug() << "setRepositoryPreferenceValues:" << query.lastError();
      return false;
    }

    return true;
  }

  bool setCommandPreferenceValues(const QString &id, const QJsonObject &preferences) {
    QSqlQuery query(db.db());
    QJsonDocument json;

    if (!query.prepare("UPDATE command SET preference_values = :preferences WHERE id = :id")) {
      qDebug() << "Failed to prepare update preference query" << query.lastError().driverText();
      return false;
    }

    json.setObject(preferences);
    query.bindValue(":preferences", json.toJson());
    query.bindValue(":id", id);

    if (!query.exec()) {
      qDebug() << "setCommandPreferenceValues:" << query.lastError().driverText();
      return false;
    }

    return true;
  }

  void setPreferenceValues(const QString &id, const QJsonObject &preferences) {
    auto cmdEntry = findCommand(id);
    QJsonObject extensionPreferences, commandPreferences;

    if (!cmdEntry) {
      qDebug() << "No command entry with ID" << id;
      return;
    };

    auto repository = findRepository(cmdEntry->repositoryId);

    for (const auto &preference : cmdEntry->command->preferences()) {
      auto &prefId = preference->name();
      bool isRepositoryPreference = false;

      if (repository) {
        for (const auto &repoPref : repository->preferences()) {
          if (repoPref->name() == prefId) {
            extensionPreferences[prefId] = preferences.value(prefId);
            isRepositoryPreference = true;
            break;
          }
        }
      }

      if (!isRepositoryPreference && preferences.contains(prefId)) {
        commandPreferences[prefId] = preferences.value(prefId);
      }
    }

    db.db().transaction();
    if (repository) { setRepositoryPreferenceValues(repository->id(), extensionPreferences); }
    setCommandPreferenceValues(id, commandPreferences);
    qDebug() << "set command prefs for" << id;
    db.db().commit();

    for (const auto &key : preferences.keys()) {}
  }

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

  void registerCommand(const QString &repositoryId, const std::shared_ptr<AbstractCmd> &cmd) {
    QSqlQuery query(db.db());

    qDebug() << "registering command with id" << cmd->id();

    query.prepare(R"(
		INSERT INTO command (id, extension_id, disabled)
		VALUES (:id, :extension_id, :disabled)
		ON CONFLICT(id) DO UPDATE SET open_count = open_count, last_opened_at = last_opened_at
		RETURNING open_count, last_opened_at
	)");
    query.bindValue(":id", cmd->id());
    query.bindValue(":extension_id", repositoryId);
    query.bindValue(":disabled", false);

    if (!query.exec() || !query.next()) { qDebug() << "Failed to register command" << cmd->id(); }
    if (!hasCommand(cmd->id())) {
      CommandDbEntry entry;

      entry.command = cmd;
      entry.disabled = false;
      entry.repositoryId = repositoryId;
      entry.openCount = query.value(0).toInt();

      qDebug() << "command" << cmd->id() << "open count" << entry.openCount;

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
  }

signals:
  void commandRegistered(const CommandDbEntry &entry) const;
};
