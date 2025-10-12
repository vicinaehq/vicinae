#pragma once
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qthreadpool.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include "ui/views/list-view.hpp"

class CalculatorHistoryView : public ListView {
public:
  CalculatorHistoryView();
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;

private:
  void handlePinned(const QString &id);
  void handleUnpinned(const QString &id);
  void handleRemoved(const QString &id);
  void handleAllRemoved();
  void generateRootList();
  void generateFilteredList(const QString &text);
  void handleCalculatorTimeout();
  void textChanged(const QString &text) override;
  void initialize() override;
  void handleComputationFinished();

  QString m_searchQuery;
  CalculatorService *m_calculator;
  QTimer *m_calcDebounce = new QTimer(this);
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calcRes;
  CalculatorWatcher m_pendingComputation;
};
