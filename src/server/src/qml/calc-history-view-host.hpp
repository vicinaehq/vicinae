#pragma once
#include "calc-history-model.hpp"
#include "list-view-host.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QFutureWatcher>
#include <QTimer>
#include <memory>
#include <vector>

class CalculatorService;

class CalcLiveSection : public SectionSource {
public:
  enum CustomRole {
    IsCalculator = Qt::UserRole + 100,
    CalcQuestion,
    CalcQuestionUnit,
    CalcAnswer,
    CalcAnswerUnit,
  };

  void setResult(std::optional<AbstractCalculatorBackend::CalculatorResult> result);
  void clear();

  QString sectionName() const override { return QStringLiteral("Calculator"); }
  int count() const override { return m_result ? 1 : 0; }

  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  QVariant customData(int i, int role) const override;

protected:
  QString itemTitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariantList itemAccessories(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_result;
};

class CalcHistoryViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  QUrl qmlComponentUrl() const override {
    return QUrl(QStringLiteral("qrc:/Vicinae/CalcHistoryListView.qml"));
  }

private:
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;

  void refresh();
  void applyGroupedData(CalculatorService::GroupedRecordList data);
  void startCalculator();
  void handleCalculatorFinished();

  CalculatorService *m_calc = nullptr;
  QString m_query;
  std::vector<std::unique_ptr<CalcHistorySection>> m_sections;

  CalcLiveSection m_liveSection;
  CalculatorWatcher m_calcWatcher;
  QTimer m_calculatorDebounce;
};
