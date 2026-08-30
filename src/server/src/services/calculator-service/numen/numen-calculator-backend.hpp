#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "numen/numen.hpp"
#include <QTimer>
#include <optional>
#include <string>

class NumenCalculatorBackend : public AbstractCalculatorBackend, QObject {
public:
  NumenCalculatorBackend();

  QString id() const override { return "numen"; }
  QString displayName() const override { return "Numen"; }
  bool isActivatable() const override { return true; }
  ComputeResult compute(const QString &question, const ComputeOptions &opts) override;
  bool supportsCurrencyConversion() const override { return true; }
  bool supportsRefreshExchangeRates() const override { return true; }
  QFuture<RefreshExchangeRatesResult> refreshExchangeRates() override;
  bool start() override;

private:
  QTimer m_rateRefreshTimer;
  numen::Numen m_numen{};
  std::optional<std::string> m_localeName;
};
