#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <libqalculate/Calculator.h>
#include <libqalculate/MathStructure.h>
#include <libqalculate/includes.h>

class QalculateBackend : public AbstractCalculatorBackend {

  QString displayName() const override;
  QString id() const override;
  bool supportsCurrencyConversion() const override;
  QFuture<RefreshExchangeRatesResult> refreshExchangeRates() override;
  bool start() override;
  ComputeResult compute(const QString &question) const override;
  QFuture<ComputeResult> asyncCompute(const QString &question) const override;
  bool supportsRefreshExchangeRates() const override;

  bool isActivatable() const override;

public:
  QalculateBackend();

private:
  static std::optional<std::string> getUnitDisplayName(const MathStructure &s, std::string_view prefix = "");

  void initializeCalculator();
  QString preprocessQuestion(const QString &question) const;

  Calculator m_calc;
  bool m_initialized = false;
  EvaluationOptions m_evalOpts;
  PrintOptions m_printOpts;
};
