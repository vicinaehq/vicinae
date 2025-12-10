#pragma once
#include "services/calculator-service/calculator-service.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "calculator-history-model.hpp"
#include <qfuturewatcher.h>
#include <qobject.h>
#include <qtimer.h>
#include <optional>
#include <ranges>
#include <string>

class CalculatorHistoryController : public QObject {
  Q_OBJECT

  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;

public:
  CalculatorHistoryController(CalculatorService *calculator, CalculatorHistoryModel *model,
                              QObject *parent = nullptr)
      : QObject(parent), m_calc(calculator), m_model(model) {
    using namespace std::chrono_literals;

    m_calculatorDebounce.setInterval(200ms);
    m_calculatorDebounce.setSingleShot(true);

    connect(m_calc, &CalculatorService::recordPinned, this, &CalculatorHistoryController::refresh);
    connect(m_calc, &CalculatorService::recordUnpinned, this, &CalculatorHistoryController::refresh);
    connect(m_calc, &CalculatorService::recordRemoved, this, &CalculatorHistoryController::refresh);
    connect(m_calc, &CalculatorService::allRecordsRemoved, this, &CalculatorHistoryController::refresh);
    connect(&m_calculatorDebounce, &QTimer::timeout, this, &CalculatorHistoryController::startCalculator);
    connect(&m_calcWatcher, &CalculatorWatcher::finished, this,
            &CalculatorHistoryController::handleCalculatorFinished);
  }

  void setFilter(const QString &query) {
    m_query = query.toStdString();
    auto grouped = m_calc->groupRecordsByTime(m_calc->query(query));
    m_model->setGroupedRecords(grouped);

    if (query.isEmpty()) {
      emit calculatorResultChanged(std::nullopt);
    } else {
      m_calculatorDebounce.start();
    }
  }

signals:
  void calculatorResultChanged(std::optional<AbstractCalculatorBackend::CalculatorResult> result);

protected:
  void refresh() { setFilter(QString::fromStdString(m_query)); }

private slots:
  void startCalculator() {
    if (m_calcWatcher.isRunning()) { m_calcWatcher.cancel(); }

    auto expression = QString::fromStdString(m_query);
    m_calculatorSearchQuery = m_query;

    if (expression.startsWith("=") && expression.size() > 1) {
      auto stripped = expression.mid(1);
      m_calcWatcher.setFuture(m_calc->backend()->asyncCompute(stripped));
      return;
    }

    bool containsNonAlnum = std::ranges::any_of(m_query, [](QChar ch) { return !ch.isLetterOrNumber(); });

    const auto isAllowedLeadingChar = [](QChar c) { return c == '(' || c == ')' || c.isLetterOrNumber(); };
    bool isComputable = expression.size() > 1 && isAllowedLeadingChar(expression.at(0)) && containsNonAlnum;

    if (!isComputable || !m_calc->backend()) { return; }

    m_calcWatcher.setFuture(m_calc->backend()->asyncCompute(expression));
  }

  void handleCalculatorFinished() {
    if (!m_calcWatcher.isFinished() || m_calculatorSearchQuery != m_query) return;
    if (auto res = m_calcWatcher.result()) { emit calculatorResultChanged(res.value()); }
  }

private:
  std::string m_query;
  std::string m_calculatorSearchQuery;
  CalculatorService *m_calc;
  CalculatorHistoryModel *m_model;
  CalculatorWatcher m_calcWatcher;
  QTimer m_calculatorDebounce;
};
