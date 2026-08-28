#include "services/calculator-service/numen/numen-calculator-backend.hpp"
#include "numen/numen.hpp"
#include "services/calculator-service/numen/numen-currency-provider.hpp"
#include "utils/environment.hpp"
#include <qfuture.h>
#include <chrono>
#include <format>

namespace {
std::string formatTimezone(const numen::Timezone &tz) {
  std::string str = tz.toString();

  const auto &info = tz.tz->get_info(std::chrono::system_clock::now());
  auto secs = info.offset;
  auto hours = std::chrono::floor<std::chrono::hours>(secs);
  auto minutes = std::chrono::floor<std::chrono::minutes>(secs - hours);

  str += std::format(" (GMT{:+}", hours.count());

  if (minutes.count()) str += std::format(":{}", minutes.count());
  if (tz.isLocalTime()) str += " — your time";

  str += ")";

  return str;
}
}; // namespace

NumenCalculatorBackend::NumenCalculatorBackend() {
  if (Environment::isAutoRateRefreshDisabled()) return;

  m_rateRefreshTimer.setInterval(std::chrono::hours{1});
  m_rateRefreshTimer.start();
  connect(&m_rateRefreshTimer, &QTimer::timeout, this, [this]() { m_numen.updateRates(); });
}

bool NumenCalculatorBackend::start() {
  auto currencyProvider = std::make_unique<NumenVicinaeCurrencyProvider>();
  m_numen.setCurrencyProvider(std::move(currencyProvider));
  return true;
}

NumenCalculatorBackend::ComputeResult NumenCalculatorBackend::compute(const QString &question,
                                                                      const ComputeOptions &opts) {

  numen::EvalOptions evalOpts{
      .parseOptions =
          {
              .strict = true,
              .locale = QLocale::system().name().toStdString(),
          },
  };

  return m_numen.compute(question.toStdString(), evalOpts)
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
              result.answer.unformatted = QString::number(n.n);
              result.answer.text = QString::fromStdString(n.text);
              if (n.unit && n.unit->resolved) {
                auto render = QString::fromStdString(n.unit->resolved->render());

                if (auto def = n.unit->def(); def && def->symbolPrefix) {
                  result.answer.text = render + QString::fromStdString(n.text);
                } else {
                  result.answer.text = QString::fromStdString(n.text) + " " + render;
                }

                if (!result.answer.unit) {
                  result.answer.unit = Unit{.displayName = QString::fromStdString(n.unit->resolved->name())};
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

QFuture<AbstractCalculatorBackend::RefreshExchangeRatesResult>
NumenCalculatorBackend::refreshExchangeRates() {
  m_numen.updateRates();
  return QtFuture::makeReadyValueFuture<RefreshExchangeRatesResult>({});
}
