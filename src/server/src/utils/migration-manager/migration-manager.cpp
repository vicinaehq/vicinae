#include "migration-manager.hpp"
#include <regex>
#include <ranges>

void MigrationManager::initialize() {
  if (!m_db.exec(R"(
      CREATE TABLE IF NOT EXISTS schema_migrations (
        id TEXT PRIMARY KEY,
        applied_at INTEGER NOT NULL,
        version INTEGER,
        checksum TEXT NOT NULL
      );
    )")) {
    qCritical() << "Failed to initialize migration manager:" << m_db.lastError().c_str();
  }
}

std::vector<MigrationManager::RegisteredMigration> MigrationManager::loadDatabaseMigrations() {
  auto stmt = m_db.prepare(R"(
    SELECT id, version, applied_at, checksum FROM schema_migrations
    ORDER BY version
  )");

  std::vector<RegisteredMigration> migrations;

  while (stmt.step()) {
    RegisteredMigration migration;

    migration.id = stmt.columnQString(0);
    migration.version = stmt.columnInt(1);
    migration.createdAt = stmt.columnUInt64(2);
    migration.checksum = stmt.columnQString(3);
    migrations.emplace_back(migration);
  }

  return migrations;
}

std::expected<MigrationManager::Migration, MigrationLoadingError>
MigrationManager::loadMigrationFile(const std::filesystem::path &path) {
  Migration migration;

  migration.id = QString::fromStdString(path.filename().string());

  {
    std::regex const filenameRegex(R"((\d+)_.*\.sql)");
    std::smatch filenameMatch;
    std::string const filename = path.filename().string();

    if (!std::regex_search(filename, filenameMatch, filenameRegex)) {
      MigrationLoadingError error;

      error.path = path;
      error.message = QString("Could not parse version from migration file name: %1").arg(filename.c_str());

      return std::unexpected(error);
    }

    migration.version = std::stoi(filenameMatch[1].str());
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) { return migration; }

  migration.content = file.readAll();

  return migration;
}

void MigrationManager::executeMigration(const Migration &migration) {
  if (!m_db.exec(migration.content.toStdString())) {
    throw std::runtime_error(
        std::format("Failed to execute migration {}: {}", migration.version, m_db.lastError()));
  }
}

QString MigrationManager::computeContentHash(const QString &content) {
  return QCryptographicHash::hash(content.toUtf8(), QCryptographicHash::Md5).toHex();
}

void MigrationManager::insertMigration(const Migration &migration) {
  auto stmt = m_db.prepare(R"(
    INSERT INTO schema_migrations (id, applied_at, version, checksum)
    VALUES (:id, :applied_at, :version, :checksum)
  )");
  stmt.bind(":id", migration.id);
  stmt.bind(":applied_at", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));
  stmt.bind(":version", migration.version);
  stmt.bind(":checksum", computeContentHash(migration.content));

  if (!stmt.exec()) {
    throw std::runtime_error(
        std::format("Failed to insert migration entry for migration: {}", migration.id.toStdString()));
  }
}

std::vector<MigrationManager::Migration> MigrationManager::loadMigrations() {
  std::filesystem::path const migrationDirPath =
      std::filesystem::path(":database") / m_migrationNamespace.toStdString() / "migrations";
  QDir const migrationDir(migrationDirPath);
  std::vector<Migration> migrations;

  for (const auto &entry : migrationDir.entryList()) {
    std::filesystem::path const migrationPath = migrationDirPath / entry.toStdString();
    auto result = loadMigrationFile(migrationPath);

    if (result) { migrations.emplace_back(*result); }
  }

  std::ranges::sort(migrations, [](auto &&a, auto &&b) { return a.version < b.version; });

  return migrations;
}

void MigrationManager::runMigrations() {
  auto dbMigrations = loadDatabaseMigrations();
  auto fsMigrations = loadMigrations();

  auto tx = m_db.transaction();

  try {
    for (size_t idx = 0; idx != fsMigrations.size(); ++idx) {
      const auto &migration = fsMigrations[idx];
      if (idx < dbMigrations.size()) {
        const auto &dbMigration = dbMigrations.at(idx);

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
    }

    if (!tx.commit()) { throw std::runtime_error("Failed to commit transaction"); }
  } catch (const std::exception &exception) {
    qCritical() << "Failed to run migrations:" << exception.what();
    tx.rollback();
  }
}

MigrationManager::MigrationManager(db::Database &db, const QString &migrationNamespace)
    : m_db(db), m_migrationNamespace(migrationNamespace) {
  initialize();
}
