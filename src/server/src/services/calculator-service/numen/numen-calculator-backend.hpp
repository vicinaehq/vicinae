#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "numen/numen.hpp"
#include "services/calculator-service/numen/numen-currency-provider.hpp"
#include <chrono>
#include <format>

namespace {
std::string formatTimezone(const numen::Timezone &tz) {
  std::string str = tz.toString();

  if (tz.isUser()) {
    str += " (Your time)";
  } else if (tz.offset.count() == 0) {
    const auto &info = tz.tz->get_info(std::chrono::system_clock::now());
    auto secs = info.offset;
    auto hours = std::chrono::floor<std::chrono::hours>(secs);
    auto minutes = std::chrono::floor<std::chrono::minutes>(secs - hours);

    str += std::format(" (GMT{:+}", hours.count());
    if (minutes.count()) str += std::format(":{}", minutes.count());
    str += ")";
  }

  return str;
}
}; // namespace

class NumenCalculatorBackend : public AbstractCalculatorBackend {
public:
  QString id() const override { return "numen"; }
  QString displayName() const override { return "Numen"; }
  bool isActivatable() const override { return true; }

  ComputeResult compute(const QString &question, const ComputeOptions &opts) override {
    return calc.compute(question.toStdString())
        .transform([&](const numen::ComputedValue &res) -> ComputeResult {
          CalculatorResult result{};

          if (res.conversion) {
            result.type = CalculatorAnswerType::CONVERSION;

            if (auto c = res.conversion->as<numen::Number::Unit>()) {
              if (auto from = c->from; from && from->resolved) {
                result.question.unit = Unit{.displayName = QString::fromStdString(from->resolved->name())};
              }
              if (auto to = c->to; to.resolved) {
                result.answer.unit = Unit{.displayName = QString::fromStdString(to.resolved->name())};
              }
            }

            if (auto c = res.conversion->as<numen::Timezone>()) {
              if (auto from = c->from; from) {
                result.question.unit = Unit{.displayName = QString::fromStdString(formatTimezone(*from))};
              }
              result.answer.unit = Unit{.displayName = QString::fromStdString(formatTimezone(c->to))};
            }
          }

          result.question.text = question;

          auto visitor = overloads{
              [&](const numen::Number &n) {
                result.answer.text = QString::fromStdString(n.text);
                if (n.unit && n.unit->resolved) {
                  result.answer.text = QString::fromStdString(n.text) + " " +
                                       QString::fromStdString(n.unit->resolved->render());
                  if (!result.answer.unit) {
                    result.answer.unit =
                        Unit{.displayName = QString::fromStdString(n.unit->resolved->name())};
                  }
                }
              },
              [&](const numen::DateTime &dt) {
                result.answer.text = QString::fromStdString(dt.toString({.withTz = false}));
                if (!result.answer.unit) {
                  numen::Timezone tz{.tz = dt.tz, .offset = dt.offset};
                  result.answer.unit = Unit{.displayName = QString::fromStdString(formatTimezone(tz))};
                }
              },
              [&](const numen::Boolean &b) { result.answer.text = b.value ? "true" : "false"; },
              [&](const numen::Duration &b) { result.answer.text = QString::fromStdString(b.toString()); }};

          std::visit(visitor, res.value);
          return result;
        })
        .value_or(ComputeResult{std::unexpected("Error")});
  }

  bool supportsCurrencyConversion() const override { return true; }
  bool supportsRefreshExchangeRates() const override { return true; }

  bool start() override {
    auto currencyProvider = std::make_unique<NumenVicinaeCurrencyProvider>();
    currencyProvider->updateRates();
    calc.setCurrencyProvider(std::move(currencyProvider));
    return true;
  }

private:
  numen::Numen calc{};
};
