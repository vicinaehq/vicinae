#pragma once
#include <filesystem>
#include <qsqldatabase.h>

class OmniDatabase {
  QSqlDatabase _db;

public:
  QSqlDatabase &db() { return _db; }

  OmniDatabase(const QString &path) : _db(QSqlDatabase::addDatabase("QSQLITE", "omni")) {
    _db.setDatabaseName(path);
    _db.open();
  }
};
