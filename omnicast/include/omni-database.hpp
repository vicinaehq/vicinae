#pragma once
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <filesystem>

class OmniDatabase {
  QSqlDatabase _db;

public:
  QSqlDatabase &db() { return _db; }

  QSqlQuery createQuery() { return QSqlQuery(_db); }

  OmniDatabase(const std::filesystem::path &path) : _db(QSqlDatabase::addDatabase("QSQLITE", "omni")) {
    std::filesystem::create_directories(path.parent_path());
    _db.setDatabaseName(path.c_str());

    if (!_db.open()) { qFatal() << "Could not open main omnicast SQLite database."; }
  }
};
