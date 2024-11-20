#pragma once
#include "config.hpp"
#include <QtSql/QSql>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qlogging.h>

struct CalculatorEntry {
  QString expression;
  QString result;
  QDateTime timestamp;
};

class CalculatorDatabase {
  QSqlDatabase db;

public:
  static CalculatorDatabase &get() {
    static CalculatorDatabase cdb(Config::dirPath() + QDir::separator() +
                                  "calculator.db");

    return cdb;
  }

public:
  CalculatorDatabase(const CalculatorDatabase &rhs) = delete;

  CalculatorDatabase(const QString &path) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
      qDebug() << "Failed to open calculator db";
    }

    QSqlQuery query(db);

    query.prepare(R"(
		CREATE TABLE IF NOT EXISTS history (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			expression TEXT NOT NULL,
			result TEXT NOT NULL,
			created_at INTEGER DEFAULT (unixepoch())
		);
	)");

    if (!query.exec()) {
      qDebug() << "Failed to execute initial query";
    }
  }

  void saveComputation(const QString &expression, const QString &result) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO history (expression, result) VALUES "
                  "(:expression, :result)");
    query.bindValue(":expression", expression);
    query.bindValue(":result", result);

    if (!query.exec()) {
      qDebug() << "query.exec() failed";
    }
  }

  QList<CalculatorEntry> list() {
    QSqlQuery query(db);

    query.exec(R"(
	  	SELECT
			expression, result, created_at
		FROM
			history
		ORDER BY created_at DESC
	  )");

    QList<CalculatorEntry> entries;

    while (query.next()) {
      uint epoch = query.value(2).toUInt();

      entries.push_back(
          CalculatorEntry{.expression = query.value(0).toString(),
                          .result = query.value(1).toString(),
                          .timestamp = QDateTime::fromSecsSinceEpoch(epoch)});
    }

    return entries;
  }
};
