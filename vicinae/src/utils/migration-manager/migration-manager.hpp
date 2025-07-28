#pragma once
#include <expected>
#include <qcryptographichash.h>
#include <qlogging.h>
#include <qsqlerror.h>
#include <qdir.h>
#include <qregularexpression.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

struct MigrationLoadingError {
  std::filesystem::path path;
  QString message;
};

class MigrationManager {
  QSqlDatabase &m_db;
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
  QStringList splitSqlStatements(const QString &content);
  QString computeContentHash(const QString &content);
  void insertMigration(const Migration &migration);

public:
  std::vector<Migration> loadMigrations();
  void runMigrations();

  MigrationManager(QSqlDatabase &db, const QString &migrationNamespace);
};
