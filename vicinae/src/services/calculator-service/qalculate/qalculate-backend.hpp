#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <libqalculate/Calculator.h>

class QalculateBackend : public AbstractCalculatorBackend {
  Calculator m_calc;

  QString displayName() const override;
  QString id() const override;
  bool supportsCurrencyConversion() const override;
  bool reloadExchangeRates() const override;
  tl::expected<CalculatorResult, CalculatorError> compute(const QString &question) const override;

  bool isActivatable() const override;

public:
  QalculateBackend();
};
