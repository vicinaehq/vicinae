#pragma once
#include "qml-command-list-model.hpp"
#include "services/calculator-service/calculator-service.hpp"

class QmlCalcHistoryModel : public QmlCommandListModel {
  Q_OBJECT

public:
  explicit QmlCalcHistoryModel(QObject *parent = nullptr);

  void initialize(ApplicationContext *ctx);
  void setFilter(const QString &text) override;
  QString searchPlaceholder() const override { return QStringLiteral("Search past calculations..."); }

protected:
  QString itemTitle(int s, int i) const override;
  QString itemIconSource(int s, int i) const override;
  QString itemAccessory(int s, int i) const override;
  std::unique_ptr<ActionPanelState> createActionPanel(int s, int i) const override;

private:
  void refresh();

  CalculatorService *m_calc = nullptr;
  CalculatorService::GroupedRecordList m_data;
  QString m_query;
};
