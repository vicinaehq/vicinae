#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <libqalculate/Calculator.h>
#include <libqalculate/MathStructure.h>

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
  void introspectUnits(MathStructure &s) const;
  void initializeCalculator();
  QString preprocessQuestion(const QString &question) const;
  static std::optional<std::string> getUnitDisplayName(const MathStructure &s, std::string_view prefix);

  Calculator m_calc;
  bool m_initialized = false;
};
