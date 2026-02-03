#pragma once
#include "extensions/calculator/history/calculator-history-controller.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qthreadpool.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include "ui/views/typed-list-view.hpp"
#include "calculator-history-model.hpp"

class CalculatorHistoryView : public TypedListView<CalculatorHistoryModel> {
public:
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override;

private:
  void textChanged(const QString &text) override;
  void initialize() override;

  CalculatorHistoryModel *m_model = nullptr;
  CalculatorHistoryController *m_controller = nullptr;
};
