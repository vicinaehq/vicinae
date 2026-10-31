#include "services/calculator-service/numen/numen-calculator-backend.hpp"
#include "numen/numen.hpp"
#include "services/calculator-service/numen/numen-currency-provider.hpp"
#include "utils/environment.hpp"
#include <qdatetime.h>
#include <qfuture.h>
#include <qlocale.h>
#include <chrono>
#include <format>
#include <variant>

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

bool isPatternSeparator(QChar c) {
  return c.isSpace() || c == QLatin1Char(',') || c == QChar(0x060C) || c == QLatin1Char(';');
}

// Keep the locale's date order and literals while omitting the weekday
// that QLocale::LongFormat typically includes.
QString stripWeekdayFields(const QString &pattern) {
  QString out;
  bool skipSeparators = false;

  for (qsizetype index = 0; index < pattern.size();) {
    const QChar c = pattern.at(index);

    if (c == QLatin1Char('\'')) {
      // copy the quoted literal verbatim, '' is an escaped quote
      out += c;
      ++index;
      while (index < pattern.size()) {
        const QChar q = pattern.at(index++);
        out += q;
        if (q != QLatin1Char('\'')) continue;
        if (index < pattern.size() && pattern.at(index) == QLatin1Char('\'')) {
          out += pattern.at(index++);
          continue;
        }
        break;
      }
      skipSeparators = false;
      continue;
    }

    if (c == QLatin1Char('d')) {
      qsizetype runLength = 0;
      while (index + runLength < pattern.size() && pattern.at(index + runLength) == QLatin1Char('d')) {
        ++runLength;
      }

      if (runLength < 3) { // d / dd is the day of the month
        out += pattern.mid(index, runLength);
        skipSeparators = false;
      } else { // ddd / dddd is the weekday
        const bool followedBySeparator =
            index + runLength < pattern.size() && isPatternSeparator(pattern.at(index + runLength));
        if (!followedBySeparator) {
          while (!out.isEmpty() && isPatternSeparator(out.back()))
            out.chop(1);
        }
        skipSeparators = true;
      }
      index += runLength;
      continue;
    }

    if (skipSeparators && isPatternSeparator(c)) {
      ++index;
      continue;
    }
    skipSeparators = false;
    out += c;
    ++index;
  }

  return out;
}

QString formatDateTime(const numen::DateTime &dt) {
  using TimeFormat = numen::DateTimeFormatOptions::TimeFormat;

  const QString neutral = QString::fromStdString(
      dt.toString({.relative = false, .withTz = false, .format = TimeFormat::Neutral}));

  // Parse as wall-clock components: QDateTime would apply the system timezone
  // and potentially shift times that fall within a DST gap.
  const QDate date = QDate::fromString(neutral.left(10), QStringLiteral("yyyy-MM-dd"));
  const QTime time = QTime::fromString(neutral.mid(11), QStringLiteral("HH:mm:ss"));

  if (!date.isValid() || !time.isValid()) { return QString::fromStdString(dt.toString({.withTz = false})); }

  const bool hasTime = time != QTime(0, 0);
  const bool isToday = date == QDate::currentDate();
  const QLocale locale;

  QString out;
  if (!isToday || !hasTime) {
    const QString pattern = stripWeekdayFields(locale.dateFormat(QLocale::LongFormat));
    out = locale.toString(date, pattern);
  }

  if (hasTime) {
    if (!out.isEmpty()) out += QLatin1Char(' ');
    out += locale.toString(time, QLocale::ShortFormat);
  }

  return out;
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
      .parseOptions = {.strict = true},
  };

  return m_numen.compute(question.toStdString(), evalOpts)
      .transform([&](const numen::ComputedValue &res) -> ComputeResult {
        CalculatorResult result{};

        if (std::holds_alternative<std::string>(res.value) && !res.conversion) {
          return std::unexpected(CalculatorError{"Cannot display string without explicit conversion"});
        }

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
              result.answer.text = formatDateTime(dt);
              if (!result.answer.unit) {
                numen::Timezone tz{.tz = dt.tz, .offset = dt.offset};
                result.answer.unit = Unit{.displayName = QString::fromStdString(formatTimezone(tz))};
              }
            },
            [&](const numen::Boolean &b) { result.answer.text = b.value ? "true" : "false"; },
            [&](const numen::Duration &b) { result.answer.text = QString::fromStdString(b.toString()); },
            [&](const std::string &str) { result.answer.text = QString::fromStdString(str); }};

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
