#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <libqalculate/Calculator.h>

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
  void initializeCalculator();

  Calculator m_calc;
  bool m_initialized = false;
};
