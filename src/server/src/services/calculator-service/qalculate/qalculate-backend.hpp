#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <libqalculate/Calculator.h>
#include <libqalculate/MathStructure.h>
#include <libqalculate/includes.h>

class QalculateBackend : public AbstractCalculatorBackend {

public:
  QalculateBackend();

  QString displayName() const override;
  QString id() const override;
  bool supportsCurrencyConversion() const override;
  QFuture<RefreshExchangeRatesResult> refreshExchangeRates() override;
  bool start() override;
  ComputeResult compute(const QString &question, const ComputeOptions &opts) override;
  QFuture<ComputeResult> asyncCompute(const QString &question, const ComputeOptions &options) override;
  void abort() override;
  void setDigitGroupingEnabled(bool value) override;
  bool supportsRefreshExchangeRates() const override;

  bool isActivatable() const override;
  bool isExpression(const std::string &query) const override;

private:
  static std::optional<std::string> getUnitDisplayName(const MathStructure &s, std::string_view prefix = "");

  void initializeCalculator();
  static QString preprocessQuestion(const QString &question);
  static QString stripTrailingOperators(QString expr);
  std::pair<std::string, PrintOptions> handleToExpression(const std::string &expression);
  std::optional<int> getTimezoneOffset(const std::string &tzName);
  void populateTzOffset();

  std::unordered_map<std::string, int> m_cityToTzMinuteOffset;
  Calculator m_calc;
  bool m_initialized = false;
  EvaluationOptions m_evalOpts;
  PrintOptions m_printOpts;
};
