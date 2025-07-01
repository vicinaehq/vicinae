#pragma once
#include "omni-database.hpp"
#include <QtNetwork/QtNetwork>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtSql/QSql>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <libqalculate/Calculator.h>
#include <libqalculate/includes.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class MyWidget : public QWidget {};

struct CalculatorEntry {
  int id;
  QString expression;
  QString result;
  QDateTime timestamp;
};

class CalculatorDatabase : public QObject {
  Calculator m_qcalc;
  OmniDatabase &m_db;

public:
  QList<CalculatorEntry> entries;

  QList<CalculatorEntry> queryAll() {
    QSqlQuery query(m_db.db());

    query.exec(R"(
	  	SELECT
			id, expression, result, created_at
		FROM
			history
		ORDER BY created_at DESC
	  )");

    QList<CalculatorEntry> entries;

    while (query.next()) {
      uint epoch = query.value(3).toUInt();

      entries.push_back(CalculatorEntry{
          .id = query.value(0).toInt(),
          .expression = query.value(1).toString(),
          .result = query.value(2).toString(),
          .timestamp = QDateTime::fromSecsSinceEpoch(epoch),
      });
    }

    return entries;
  }

public:
  CalculatorDatabase(const CalculatorDatabase &rhs) = delete;

  CalculatorDatabase(OmniDatabase &db) : m_db(db) {
    m_qcalc.checkExchangeRatesDate(1);
    m_qcalc.loadExchangeRates();
    m_qcalc.loadGlobalDefinitions();
    m_qcalc.loadLocalDefinitions();

    entries = queryAll();
  }

  /**
   * high level function to evaluate a math expression
   */
  std::pair<std::string, bool> quickCalculate(const std::string &expression) {

    EvaluationOptions evalOpts;

    evalOpts.auto_post_conversion = POST_CONVERSION_BEST;
    evalOpts.structuring = STRUCTURING_SIMPLIFY;
    evalOpts.parse_options.limit_implicit_multiplication = true;
    evalOpts.parse_options.parsing_mode = PARSING_MODE_CONVENTIONAL;
    evalOpts.parse_options.units_enabled = true;
    evalOpts.parse_options.unknowns_enabled = false;

    MathStructure result = m_qcalc.calculate(expression, evalOpts);

    if (result.containsUnknowns()) { return {"", false}; }

    bool error = false;

    for (auto msg = m_qcalc.message(); msg; msg = m_qcalc.nextMessage()) {
      qCritical() << "Calculator Error" << msg->message();
      error = true;
    }

    if (error) return {"", false};

    PrintOptions printOpts;

    printOpts.indicate_infinite_series = true;
    printOpts.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
    printOpts.use_unicode_signs = true;

    std::string res = result.print(printOpts);

    return {res, true};
  }

  void insertComputation(const QString &expression, const QString &result) {
    QSqlQuery query(m_db.db());

    query.prepare("INSERT INTO history (expression, result) VALUES "
                  "(:expression, :result) RETURNING id, expression, result, created_at");
    query.bindValue(":expression", expression);
    query.bindValue(":result", result);

    if (!query.exec()) { qDebug() << "query.exec() failed"; }

    if (!query.next()) return;

    uint epoch = query.value(3).toUInt();
    auto entry = CalculatorEntry{
        .id = query.value(0).toInt(),
        .expression = query.value(1).toString(),
        .result = query.value(2).toString(),
        .timestamp = QDateTime::fromSecsSinceEpoch(epoch),
    };

    entries.push_front(entry);
  }

  QList<CalculatorEntry> listAll() { return entries; }

  bool removeById(int id) {
    QSqlQuery query(m_db.db());

    for (int i = 0; i != entries.size(); ++i) {
      if (entries.at(i).id == id) {
        entries.removeAt(i);
        break;
      }
    }

    query.prepare("DELETE FROM history WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
  }

public slots:
  void requestFinished(QNetworkReply *reply) {}
};
