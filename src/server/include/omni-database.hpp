#pragma once
#include "db/database.hpp"
#include "utils/migration-manager/migration-manager.hpp"
#include <qlogging.h>
#include <filesystem>

static constexpr std::string_view OMNI_PRAGMAS[] = {"PRAGMA foreign_keys = ON;"};

class OmniDatabase {
  db::Database _db;

public:
  db::Database &db() { return _db; }

  OmniDatabase(const std::filesystem::path &path) {
    auto result = db::Database::open(path);

    if (!result) {
      qCritical() << "Could not open main omnicast SQLite database:" << result.error().c_str();
      return;
    }

    _db = std::move(*result);

    for (const auto &pragma : OMNI_PRAGMAS) {
      _db.exec(pragma);
    }

    MigrationManager manager(_db, "omnicast");

    manager.runMigrations();
  }
};
