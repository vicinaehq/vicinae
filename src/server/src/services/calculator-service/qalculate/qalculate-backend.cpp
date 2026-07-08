#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QRegularExpression>
#include <QTime>
#include <QTimeZone>
#include <cctype>
#include <format>
#include <libqalculate/MathStructure.h>
#include <libqalculate/QalculateDateTime.h>
#include <libqalculate/includes.h>
#include <libqalculate/Unit.h>
#include <libqalculate/Variable.h>
#include <libqalculate/Function.h>
#include <libqalculate/Prefix.h>
#include <qlogging.h>
#include <QtConcurrent/QtConcurrent>
#include <qobjectdefs.h>

using CalculatorResult = QalculateBackend::CalculatorResult;
using CalculatorError = QalculateBackend::CalculatorError;

namespace {

// Lowercased, with underscores treated as spaces, so "New_York" matches "new york".
std::string normalizeCityKey(std::string_view city) {
  std::string key;
  key.reserve(city.size());
  for (char c : city) {
    key.push_back(c == '_' ? ' ' : static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  }
  return key;
}

} // namespace

QalculateBackend::QalculateBackend() = default;

bool QalculateBackend::isActivatable() const { return true; }

bool QalculateBackend::start() {
  if (!m_initialized) {
    initializeCalculator();
    m_initialized = true;
  }

  return true;
}

void QalculateBackend::populateTzOffset() {
  if (!m_cityToTzMinuteOffset.empty()) { return; }

  const QDateTime now = QDateTime::currentDateTimeUtc();

  for (const auto &tz : QTimeZone::availableTimeZoneIds()) {
    const std::string tzcode(tz.constData(), tz.length());

    // Last path segment, so "America/Argentina/Buenos_Aires" -> "Buenos_Aires".
    const size_t slashPos = tzcode.rfind('/');
    const std::string cityName = slashPos != std::string::npos ? tzcode.substr(slashPos + 1) : tzcode;

    const int offsetMinutes = QTimeZone(tz).offsetFromUtc(now) / 60;
    m_cityToTzMinuteOffset[normalizeCityKey(cityName)] = offsetMinutes;
  }
}

std::optional<int> QalculateBackend::getTimezoneOffset(const std::string &tzName) {
  // Custom offset format: +HH:MM or -HH:MM
  if (tzName.size() >= 6 && (tzName.front() == '+' || tzName.front() == '-')) {
    const int sign = tzName.front() == '+' ? 1 : -1;
    const QTime offset = QTime::fromString(QString::fromStdString(tzName.substr(1)), "HH:mm");
    if (offset.isValid()) { return sign * (offset.hour() * 60 + offset.minute()); }
    return std::nullopt;
  }

  this->populateTzOffset();

  if (auto it = m_cityToTzMinuteOffset.find(normalizeCityKey(tzName)); it != m_cityToTzMinuteOffset.end()) {
    return it->second;
  }

  if (QTimeZone tz(QString::fromStdString(tzName).toUtf8()); tz.isValid()) {
    return tz.offsetFromUtc(QDateTime::currentDateTimeUtc()) / 60;
  }

  return std::nullopt;
}

std::pair<std::string, PrintOptions> QalculateBackend::handleToExpression(const std::string &expression) {
  PrintOptions resultPrintOpts = m_printOpts;

  std::string toExpression;
  std::string calcExpression = expression;

  if (CALCULATOR->separateToExpression(calcExpression, toExpression, m_evalOpts, true)) {
    if (!toExpression.empty()) {
      const std::string toStr = QString::fromStdString(toExpression).trimmed().toStdString();

      if (toStr == "hex" || toStr == "hexadecimal") {
        resultPrintOpts.base = BASE_HEXADECIMAL;
      } else if (toStr == "bin" || toStr == "binary") {
        resultPrintOpts.base = BASE_BINARY;
      } else if (toStr == "dec" || toStr == "decimal") {
        resultPrintOpts.base = BASE_DECIMAL;
        resultPrintOpts.min_exp = EXP_NONE;
      } else if (toStr == "oct" || toStr == "octal") {
        resultPrintOpts.base = BASE_OCTAL;
      } else if (toStr == "roman") {
        resultPrintOpts.base = BASE_ROMAN_NUMERALS;
      } else if (toStr == "time") {
        resultPrintOpts.base = BASE_TIME;
      } else if (toStr == "unicode") {
        resultPrintOpts.base = BASE_UNICODE;
      } else if (toStr == "sci" || toStr == "scientific") {
        resultPrintOpts.sort_options.minus_last = false;
        resultPrintOpts.min_exp = EXP_PURE;
        resultPrintOpts.show_ending_zeroes = true;
        resultPrintOpts.use_unit_prefixes = false;
        resultPrintOpts.negative_exponents = true;
      } else if (toStr == "eng" || toStr == "engineering") {
        resultPrintOpts.sort_options.minus_last = false;
        resultPrintOpts.min_exp = EXP_BASE_3;
        resultPrintOpts.show_ending_zeroes = true;
        resultPrintOpts.use_unit_prefixes = false;
        resultPrintOpts.negative_exponents = false;
      } else if (toStr == "simple") {
        resultPrintOpts.sort_options.minus_last = true;
        resultPrintOpts.min_exp = EXP_NONE;
        resultPrintOpts.show_ending_zeroes = false;
        resultPrintOpts.use_unit_prefixes = true;
        resultPrintOpts.negative_exponents = false;
      } else if (toStr == "utc" || toStr == "gmt") {
        resultPrintOpts.time_zone = TIME_ZONE_UTC;
      } else {
        // Try to look up as timezone offset or IANA timezone name
        if (auto offset = getTimezoneOffset(toStr)) {
          resultPrintOpts.time_zone = TIME_ZONE_CUSTOM;
          resultPrintOpts.custom_time_zone = *offset;
        } else {
          // Not a display format or timezone - it's a unit conversion, restore the full expression
          calcExpression = expression;
        }
      }
    }
  }

  return {calcExpression, resultPrintOpts};
}

std::expected<CalculatorResult, CalculatorError> QalculateBackend::compute(const QString &question,
                                                                           const ComputeOptions &opts) {

  const auto fail = [](auto &&reason) { return std::unexpected(CalculatorError(reason)); };

  if (opts.mode == ComputeMode::MixedSearch) {
    const auto isAllowedLeadingChar = [](QChar c) {
      return c == '-' || c == '(' || c == ')' || c.isLetterOrNumber() ||
             c.category() == QChar::Symbol_Currency;
    };
    bool const isMixedSearchComputable =
        question.size() > 1 && isAllowedLeadingChar(question.at(0)) && isExpression(question.toStdString());

    if (!isMixedSearchComputable) { return fail("Not a valid expression"); }
  }

  QString expression = preprocessQuestion(question);
  expression = stripTrailingOperators(expression);
  if (expression.isEmpty()) return std::unexpected(CalculatorError("Empty expression"));

  auto stdExpr = expression.toStdString();
  std::string const localizedExpression = CALCULATOR->unlocalizeExpression(stdExpr);

  // Handle "to" expressions and get the calculation expression + display options
  auto [calcExpression, resultPrintOpts] = handleToExpression(localizedExpression);

  MathStructure in = CALCULATOR->parse(calcExpression);
  MathStructure result;
  CALCULATOR->calculate(&result, calcExpression, 10000, m_evalOpts, &in);

  if (CALCULATOR->aborted()) return std::unexpected(CalculatorError("Computation aborted"));

  if (result.containsUnknowns()) { return std::unexpected(CalculatorError("Unknown component in question")); }

  bool error = false;

  for (auto msg = CALCULATOR->message(); msg; msg = CALCULATOR->nextMessage()) {
    if (msg->type() == MESSAGE_ERROR) { error = true; }
  }

  if (error) return std::unexpected(CalculatorError("Calculation error"));

  // Format question with original print options (no base conversion)
  in.format(m_printOpts);

  PrintOptions parsedPrintOpts = m_printOpts;
  parsedPrintOpts.excessive_parenthesis = true;
  std::string parsedExprText = in.print(parsedPrintOpts);

  // Format result with potentially modified print options (with base conversion)
  result.format(resultPrintOpts);
  std::string res = result.print(resultPrintOpts);

  CalculatorResult calcRes;
  if (result.containsType(STRUCT_UNIT)) {
    if (auto unit = getUnitDisplayName(in)) { calcRes.question.unit = Unit{.displayName = unit->c_str()}; }
    if (auto unit = getUnitDisplayName(result)) { calcRes.answer.unit = Unit{.displayName = unit->c_str()}; }
    calcRes.type = CalculatorAnswerType::CONVERSION;
  } else {
    calcRes.type = CalculatorAnswerType::NORMAL;
  }

  calcRes.question.text = QString::fromStdString(parsedExprText);
  calcRes.answer.text = QString::fromStdString(res);

  return calcRes;
}

QString QalculateBackend::stripTrailingOperators(QString expr) {
  static constexpr std::string_view ops = "+-*/^&|!<>=~\\(";
  while (!expr.isEmpty()) {
    QChar last = expr.back();
    if (last.unicode() < 128 && ops.find(static_cast<char>(last.unicode())) != std::string_view::npos) {
      expr.chop(1);
    } else {
      break;
    }
  }
  return expr.trimmed();
}

bool QalculateBackend::isExpression(const std::string &query) const {
  QString expression = preprocessQuestion(QString::fromStdString(query));
  expression = stripTrailingOperators(expression);
  if (expression.isEmpty()) return false;

  auto stdExpr = expression.toStdString();
  std::string const localized = CALCULATOR->unlocalizeExpression(stdExpr);
  MathStructure parsed = CALCULATOR->parse(localized, m_evalOpts.parse_options);

  CALCULATOR->clearMessages();

  bool hasDigit = std::ranges::any_of(stdExpr, [](unsigned char c) { return std::isdigit(c); });
  if (hasDigit && parsed.containsType(STRUCT_UNIT) && parsed.containsType(STRUCT_NUMBER)) return true;

  static constexpr std::string_view ARITHMETIC_OPS = "+-*/^%";
  for (size_t i = 1; i < stdExpr.size(); ++i) {
    if (ARITHMETIC_OPS.find(stdExpr[i]) != std::string_view::npos) return true;
  }
  if (stdExpr.find('(') != std::string::npos) return true;
  if (stdExpr.find(" to ") != std::string::npos) return true;

  return false;
}

QString QalculateBackend::preprocessQuestion(const QString &query) {
  QString q = query.simplified();

  // Rewrite " in " to " to " for unit conversion (avoids "in" being parsed as inches)
  if (int idx = q.indexOf(QStringLiteral(" in ")); idx > 0 && idx + 4 < q.size()) {
    q.replace(idx, 4, QStringLiteral(" to "));
  }

  struct Rule {
    QRegularExpression re;
    QString replacement;
  };
  static const auto storageNorms = std::array{
      Rule{QRegularExpression(R"((?<![a-zA-Z])tb(?![a-zA-Z]))"), "TB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])gb(?![a-zA-Z]))"), "GB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])mb(?![a-zA-Z]))"), "MB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])kb(?![a-zA-Z]))"), "kB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])pb(?![a-zA-Z]))"), "PB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])eb(?![a-zA-Z]))"), "EB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])tib(?![a-zA-Z]))"), "TiB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])gib(?![a-zA-Z]))"), "GiB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])mib(?![a-zA-Z]))"), "MiB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])kib(?![a-zA-Z]))"), "KiB"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])tbit(?![a-zA-Z]))"), "Tbit"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])gbit(?![a-zA-Z]))"), "Gbit"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])mbit(?![a-zA-Z]))"), "Mbit"},
      Rule{QRegularExpression(R"((?<![a-zA-Z])kbit(?![a-zA-Z]))"), "kbit"},
  };

  for (const auto &[re, replacement] : storageNorms) {
    q.replace(re, replacement);
  }

  return q;
}

QFuture<QalculateBackend::ComputeResult> QalculateBackend::asyncCompute(const QString &question,
                                                                        const ComputeOptions &opts) {
  return QtConcurrent::run([this, question, opts]() -> ComputeResult { return compute(question, opts); });
}

void QalculateBackend::abort() { CALCULATOR->abort(); }

void QalculateBackend::setDigitGroupingEnabled(bool value) {
  m_printOpts.digit_grouping = value ? DIGIT_GROUPING_LOCALE : DIGIT_GROUPING_NONE;
}

QString QalculateBackend::id() const { return "qalculate"; }

QString QalculateBackend::displayName() const { return "Qalculate!"; }

bool QalculateBackend::supportsRefreshExchangeRates() const { return true; }

QFuture<AbstractCalculatorBackend::RefreshExchangeRatesResult> QalculateBackend::refreshExchangeRates() {
  qInfo() << "Refreshing Qalculate exchange rates...";

  return QtConcurrent::run([this]() -> RefreshExchangeRatesResult {
    if (!CALCULATOR->fetchExchangeRates()) {
      qWarning() << "Failed to fetch exchange rates";
      return std::unexpected("Failed to fetch exchange rates");
    }
    m_calc.loadExchangeRates();
    qInfo() << "Done refreshing exchange rates";
    return {};
  });
}

bool QalculateBackend::supportsCurrencyConversion() const { return true; }

void QalculateBackend::initializeCalculator() {
  m_evalOpts.auto_post_conversion = POST_CONVERSION_BEST;
  m_evalOpts.structuring = STRUCTURING_SIMPLIFY;
  m_evalOpts.parse_options.parsing_mode = PARSING_MODE_ADAPTIVE;
  m_evalOpts.parse_options.units_enabled = true;
  m_evalOpts.parse_options.unknowns_enabled = false;
  m_evalOpts.local_currency_conversion = true;

  m_printOpts.indicate_infinite_series = false;
  m_printOpts.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
  m_printOpts.use_unicode_signs = true;
  m_printOpts.base_display = BASE_DISPLAY_NORMAL;
  m_printOpts.number_fraction_format = FRACTION_DECIMAL;
  m_printOpts.abbreviate_names = true;
  m_printOpts.use_unit_prefixes = true;
  m_printOpts.place_units_separately = true;
  m_printOpts.short_multiplication = true;
  m_printOpts.show_ending_zeroes = true;
  m_printOpts.min_exp = EXP_PRECISION;

  m_calc.reset();
  m_calc.loadExchangeRates();
  m_calc.loadGlobalDefinitions();
  m_calc.loadLocalDefinitions();

  CALCULATOR->setTemperatureCalculationMode(TEMPERATURE_CALCULATION_HYBRID);
  CALCULATOR->setPrecision(10);
}

std::optional<std::string> QalculateBackend::getUnitDisplayName(const MathStructure &s,
                                                                std::string_view prefix) {
  if (prefix.empty() && s.prefix()) {
    std::string_view const prefix = s.prefix()->preferredDisplayName().name;
    if (!prefix.empty()) { return getUnitDisplayName(s, prefix); }
  }

  if (auto unit = s.unit()) {
    return std::format("{}{}", prefix, unit->preferredDisplayName(false, false, true, false).name);
  }

  for (size_t i = 0; i != s.size(); ++i) {
    if (auto unit = getUnitDisplayName(s[i], prefix)) { return unit; }
  }

  return std::nullopt;
}
