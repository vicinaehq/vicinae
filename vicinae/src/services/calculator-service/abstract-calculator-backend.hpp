#pragma once
#include <qfuture.h>
#include <qstring.h>
#include "utils/expected.hpp"

class AbstractCalculatorBackend {
public:
  enum CalculatorAnswerType {
    NORMAL,    // regular arithmetic
    CONVERSION // unit/currency conversion
  };

  struct Unit {
    QString displayName;
  };

  struct CalculatorResult {
    CalculatorAnswerType type;
    struct {
      QString text;
      std::optional<Unit> unit;
    } question;

    struct {
      QString text;
      std::optional<Unit> unit;
    } answer;
  };

  struct CalculatorError {
    QString m_message;

    const QString &message() const { return m_message; }

    CalculatorError(const QString &message) : m_message(message) {}
  };

  using ComputeResult = tl::expected<CalculatorResult, CalculatorError>;
  using RefreshExchangeRatesResult = tl::expected<void, std::string>;

  virtual QString id() const = 0;
  virtual QString displayName() const { return id(); }

  virtual ComputeResult compute(const QString &question) const = 0;
  virtual QFuture<ComputeResult> asyncCompute(const QString &question) const = 0;

  virtual bool supportsCurrencyConversion() const { return false; }

  virtual bool supportsRefreshExchangeRates() const { return false; }

  /**
   * Refresh currency exchange rates.
   *
   * If the backend implements this, it also need to impleemnt `supportsRefreshExchangeRates` to explicitly
   * indicate that it supports it.
   *
   * Returns a future that fullfills when the rates have been refreshed or an error occured.
   * An optional error is returned.
   */
  virtual QFuture<RefreshExchangeRatesResult> refreshExchangeRates() {
    QPromise<RefreshExchangeRatesResult> promise;
    promise.addResult(tl::unexpected("Not implemented"));
    promise.finish();
    return promise.future();
  }

  /**
   * Whether this calculator implementation can be used in the current environment.
   * Called before `start`.
   */
  virtual bool isActivatable() const = 0;

  /**
   * The calculator backend has been selected and can now start.
   * Use this to perform synchronous initialization stuff.
   * The returned boolean indicates whether the initialization was successful.
   */
  virtual bool start() { return true; }

  /**
   * The calculator backend has been deselected, this one is stopped for another one to be started in its
   * stead. It's okay to not cleanup stuff here as hot backend swapping is not frequent, but do it if you can.
   */
  virtual void stop() {}

  virtual ~AbstractCalculatorBackend() = default;
};
