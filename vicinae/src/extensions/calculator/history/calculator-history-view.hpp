#pragma once
#include "extensions/calculator/history/calculator-history-controller.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "ui/views/typed-list-view.hpp"
#include "calculator-history-model.hpp"
#include <optional>

class TransformResult;

class CalculatorHistoryView : public TypedListView<CalculatorHistoryModel> {
public:
  CalculatorHistoryView();

protected:
  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override;
  QWidget *wrapUI(QWidget *content) override;
  bool inputFilter(QKeyEvent *event) override;

private:
  void textChanged(const QString &text) override;
  void initialize() override;
  void onCalculatorResultChanged(std::optional<AbstractCalculatorBackend::CalculatorResult> result);
  void selectCalculatorResult();
  void deselectCalculatorResult();
  void activateCalculatorResult();
  void updateCalculatorResultActions();

  CalculatorHistoryModel *m_model = nullptr;
  CalculatorHistoryController *m_controller = nullptr;
  TransformResult *m_calcResult = nullptr;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_currentResult;
  bool m_calcResultSelected = false;
};
