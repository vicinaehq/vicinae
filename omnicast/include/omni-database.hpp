#pragma once
#include <qsqldatabase.h>
#include <qsqlquery.h>

class OmniDatabase {
  QSqlDatabase _db;

public:
  QSqlDatabase &db() { return _db; }

  QSqlQuery createQuery() { return QSqlQuery(_db); }

  OmniDatabase(const QString &path) : _db(QSqlDatabase::addDatabase("QSQLITE", "omni")) {
    _db.setDatabaseName(path);
    _db.open();
  }
};
