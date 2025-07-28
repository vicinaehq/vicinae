#pragma once
#include <qstring.h>
#include <expected>

class AbstractCalculatorBackend {
public:
  enum CalculatorAnswerType {
    NORMAL,    // regular arithmetic
    CONVERSION // unit/currency conversion
  };

  struct CalculatorResult {
    QString question;
    QString answer;
    CalculatorAnswerType type;
  };

  struct CalculatorError {
    QString m_message;

    const QString &message() const { return m_message; }

    CalculatorError(const QString &message) : m_message(message) {}
  };

  virtual QString name() const = 0;
  virtual std::expected<CalculatorResult, CalculatorError> compute(const QString &question) const = 0;

  virtual bool supportsCurrencyConversion() const { return false; }
  virtual bool reloadExchangeRates() const { return false; }

  virtual ~AbstractCalculatorBackend() = default;
};
