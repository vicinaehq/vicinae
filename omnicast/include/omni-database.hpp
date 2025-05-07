#pragma once
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

class OmniDatabase {
  QSqlDatabase _db;

public:
  QSqlDatabase &db() { return _db; }

  QSqlQuery createQuery() { return QSqlQuery(_db); }

  OmniDatabase(const QString &path) : _db(QSqlDatabase::addDatabase("QSQLITE", "omni")) {
    _db.setDatabaseName(path);
    if (!_db.open()) { qFatal() << "Could not open main omnicast SQLite database."; }
  }
};
