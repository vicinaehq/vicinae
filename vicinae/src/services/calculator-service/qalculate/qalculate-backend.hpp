#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <libqalculate/Calculator.h>

class QalculateBackend : public AbstractCalculatorBackend {
  Calculator m_calc;

  QString name() const override;
  bool supportsCurrencyConversion() const override;
  bool reloadExchangeRates() const override;
  std::expected<CalculatorResult, CalculatorError> compute(const QString &question) const override;

public:
  QalculateBackend();
};
