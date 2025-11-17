#include "migration-manager.hpp"
#include <qsqldatabase.h>
#include <regex>
#include <ranges>

void MigrationManager::initialize() {
  QSqlQuery query(m_db);
  bool created = query.exec(R"(
	  	CREATE TABLE IF NOT EXISTS schema_migrations (
			id TEXT PRIMARY KEY,
			applied_at INTEGER NOT NULL,
			version INTEGER,
			checksum TEXT NOT NULL
		);
	  )");

  if (!created) { qCritical() << "Failed to initialize migration manager" << query.lastError(); }
}

std::vector<MigrationManager::RegisteredMigration> MigrationManager::loadDatabaseMigrations() {
  QSqlQuery query(m_db);
  const char *QUERY = R"(
		SELECT id, version, applied_at, checksum FROM schema_migrations
		ORDER BY version
	)";

  if (!query.exec(QUERY)) { return {}; }

  std::vector<RegisteredMigration> migrations;

  while (query.next()) {
    RegisteredMigration migration;

    migration.id = query.value(0).toString();
    migration.version = query.value(1).toInt();
    migration.createdAt = query.value(2).toULongLong();
    migration.checksum = query.value(3).toString();
    migrations.emplace_back(migration);
  }

  return migrations;
}

tl::expected<MigrationManager::Migration, MigrationLoadingError>
MigrationManager::loadMigrationFile(const std::filesystem::path &path) {
  Migration migration;

  migration.id = path.filename().c_str();

  {
    std::regex filenameRegex(R"((\d+)_.*\.sql)");
    std::smatch filenameMatch;
    std::string filename = path.filename();

    if (!std::regex_search(filename, filenameMatch, filenameRegex)) {
      MigrationLoadingError error;

      error.path = path;
      error.message = QString("Could not parse version from migration file name: %1").arg(filename.c_str());

      return tl::unexpected(error);
    }

    migration.version = std::stoi(filenameMatch[1].str());
  }

  QFile file(path.c_str());

  if (!file.open(QIODevice::ReadOnly)) { return migration; }

  migration.content = file.readAll();

  return migration;
}

void MigrationManager::executeMigration(const Migration &migration) {
  // The sqlite driver does not support executing multiple statements at once
  // so we need this manual splitting.
  QStringList statements = splitSqlStatements(migration.content);

  for (const QString &statement : statements) {
    QString trimmed = statement.trimmed();
    if (trimmed.isEmpty() || trimmed.startsWith("--")) { continue; }

    qDebug() << "executing" << statement;

    QSqlQuery query(m_db);
    if (!query.exec(trimmed)) {
      auto error = std::format("Failed to execute statement in migration {}: {}", migration.version,
                               query.lastError().databaseText().toStdString());

      throw std::runtime_error(error);
    }
  }
}

QStringList MigrationManager::splitSqlStatements(const QString &content) {
  QStringList statements;
  QString current;
  QStringList lines = content.split('\n');

  for (const QString &line : lines) {
    QString trimmed = line.trimmed();

    // Skip comment lines
    if (trimmed.startsWith("--") || trimmed.isEmpty()) { continue; }

    current += line + "\n";

    // Check if line ends with semicolon (simple approach)
    if (trimmed.endsWith(';')) {
      statements.append(current.trimmed());
      current.clear();
    }
  }

  // Add any remaining content
  if (!current.trimmed().isEmpty()) { statements.append(current.trimmed()); }

  return statements;
}

QString MigrationManager::computeContentHash(const QString &content) {
  return QCryptographicHash::hash(content.toUtf8(), QCryptographicHash::Md5).toHex();
}

void MigrationManager::insertMigration(const Migration &migration) {
  QSqlQuery query(m_db);

  query.prepare(R"(
	  	INSERT INTO schema_migrations (id, applied_at, version, checksum)
		VALUES (:id, :applied_at, :version, :checksum)
	  )");
  query.bindValue(":id", migration.id);
  query.bindValue(":applied_at", QDateTime::currentSecsSinceEpoch());
  query.bindValue(":version", migration.version);
  query.bindValue(":checksum", computeContentHash(migration.content));

  if (!query.exec()) {
    throw std::runtime_error(
        std::format("Failed to insert migration entry for migration: {}", migration.id.toStdString()));
  }
}

std::vector<MigrationManager::Migration> MigrationManager::loadMigrations() {
  std::filesystem::path migrationDirPath =
      std::filesystem::path(":database") / m_migrationNamespace.toStdString() / "migrations";
  QDir migrationDir(migrationDirPath);
  std::vector<Migration> migrations;

  for (const auto &entry : migrationDir.entryList()) {
    std::filesystem::path migrationPath = migrationDirPath / entry.toStdString();
    auto result = loadMigrationFile(migrationPath);

    if (result) { migrations.emplace_back(*result); }
  }

  std::ranges::sort(migrations, [](auto &&a, auto &&b) { return a.version < b.version; });

  return migrations;
}

void MigrationManager::runMigrations() {
  auto dbMigrations = loadDatabaseMigrations();
  auto fsMigrations = loadMigrations();

  if (!m_db.transaction()) {
    qCritical() << "Failed to start" << m_db.lastError();
    return;
  }

  size_t newExecCount = 0;

  try {
    for (size_t idx = 0; idx != fsMigrations.size(); ++idx) {
      const auto &migration = fsMigrations[idx];
      if (idx < dbMigrations.size()) {
        auto dbMigration = dbMigrations.at(idx);

        if (dbMigration.version != migration.version) {
          throw std::runtime_error("Migration version mismatch");
        }

        continue;
      }

      if (!dbMigrations.empty() && dbMigrations.back().version >= migration.version) {
        throw std::runtime_error(
            "New migration should have greater version than the last database migration");
      }

      qInfo() << "Applying migration" << migration.id;
      executeMigration(migration);
      insertMigration(migration);
      ++newExecCount;
    }

    if (!m_db.commit()) { throw std::runtime_error("Failed to commit transaction"); }
  } catch (const std::exception &exception) {
    qCritical() << "Failed to run migrations:" << exception.what();
    m_db.rollback();
  }
}

MigrationManager::MigrationManager(QSqlDatabase &db, const QString &migrationNamespace)
    : m_db(db), m_migrationNamespace(migrationNamespace) {
  initialize();
}
