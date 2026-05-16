#pragma once
#include <cctype>
#include <qfuture.h>
#include <qstring.h>
#include <expected>
#include <ranges>
#include <string>
#include <string_view>

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

  using ComputeResult = std::expected<CalculatorResult, CalculatorError>;
  using RefreshExchangeRatesResult = std::expected<void, std::string>;

  virtual QString id() const = 0;
  virtual QString displayName() const { return id(); }

  virtual ComputeResult compute(const QString &question) = 0;
  virtual QFuture<ComputeResult> asyncCompute(const QString &question) = 0;

  virtual void abort() {}

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
    promise.addResult(std::unexpected("Not implemented"));
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

  virtual bool isExpression(const std::string &query) const {
    if (query.starts_with("0x") || query.starts_with("0b") || query.starts_with("0o")) return true;

    static constexpr std::string_view ALWAYS_BINARY = "*/^%";
    for (size_t i = 1; i < query.size(); ++i) {
      auto cur = static_cast<unsigned char>(query[i]);
      auto prev = static_cast<unsigned char>(query[i - 1]);
      if (ALWAYS_BINARY.find(query[i]) != std::string_view::npos) return true;
      if ((cur == '+' || cur == '-') && (std::isalnum(prev) || query[i - 1] == ')')) return true;
      if (cur == '(' || prev == '(') return true;
    }

    auto hasConversion = [&](std::string_view keyword) {
      auto pos = query.find(keyword);
      if (pos == std::string_view::npos || pos == 0 || pos + keyword.size() >= query.size()) return false;
      auto before = query.substr(0, pos);
      return std::ranges::any_of(before, [](unsigned char c) { return std::isdigit(c); });
    };
    return hasConversion(" to ") || hasConversion(" in ");
  }

  virtual ~AbstractCalculatorBackend() = default;
};
