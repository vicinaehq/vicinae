#pragma once
#include <expected>
#include <qcryptographichash.h>
#include <qdir.h>
#include <qlogging.h>
#include <qregularexpression.h>

#include "db/database.hpp"

struct MigrationLoadingError {
  std::filesystem::path path;
  QString message;
};

class MigrationManager {
  db::Database &m_db;
  QString m_migrationNamespace;

  struct Migration {
    QString id;
    int version = -1;
    QString content;
  };

  struct RegisteredMigration {
    QString id;
    int version = -1;
    unsigned long long createdAt;
    QString checksum;
  };

  void initialize();
  std::vector<RegisteredMigration> loadDatabaseMigrations();
  std::expected<Migration, MigrationLoadingError> loadMigrationFile(const std::filesystem::path &path);

  void executeMigration(const Migration &migration);
  QString computeContentHash(const QString &content);
  void insertMigration(const Migration &migration);

public:
  std::vector<Migration> loadMigrations();
  void runMigrations();

  MigrationManager(db::Database &db, const QString &migrationNamespace);
};
