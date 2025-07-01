#pragma once
#include "common.hpp"
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

  std::vector<CommandDbEntry> entries;
  std::vector<std::shared_ptr<AbstractCommandRepository>> repositories;

  const AbstractCommandRepository *findRepository(const QString &id) {
    for (const auto &repo : repositories) {
      if (repo->id() == id) return repo.get();
    }

    return nullptr;
  }

public:
  std::vector<CommandDbEntry> commands() const { return entries; }

  OmniCommandDatabase() {}

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
    qDebug() << "registering command with id" << cmd->uniqueId();

    if (!hasCommand(cmd->uniqueId())) {
      CommandDbEntry entry;

      entry.command = cmd;
      entry.disabled = false;
      entry.repositoryId = repositoryId;

      qDebug() << "command" << cmd->uniqueId() << "open count" << entry.openCount;

      // TODO: use something better
      entries.insert(entries.begin(), entry);
      emit commandRegistered(entry);
    }
  }

  void registerRepository(const std::shared_ptr<AbstractCommandRepository> &repository) {
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

    emit registryAdded(repository);
  }

signals:
  void commandRegistered(const CommandDbEntry &entry) const;
  void registryAdded(const std::shared_ptr<AbstractCommandRepository> &registry) const;
};
