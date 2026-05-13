#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QDebug>
#include <QRegularExpression>
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
#include <ranges>

using CalculatorResult = QalculateBackend::CalculatorResult;
using CalculatorError = QalculateBackend::CalculatorError;

QalculateBackend::QalculateBackend() = default;

bool QalculateBackend::isActivatable() const { return true; }

bool QalculateBackend::start() {
  if (!m_initialized) {
    initializeCalculator();
    m_initialized = true;
  }

  return true;
}

std::expected<CalculatorResult, CalculatorError> QalculateBackend::compute(const QString &question) {
  QString expression = preprocessQuestion(question);
  expression = stripTrailingOperators(expression);
  if (expression.isEmpty()) return std::unexpected(CalculatorError("Empty expression"));

  auto stdExpr = expression.toStdString();
  if (!isComputableExpression(stdExpr)) return std::unexpected(CalculatorError("Not computable"));

  std::string const localizedExpression = CALCULATOR->unlocalizeExpression(stdExpr);
  std::string res = CALCULATOR->calculateAndPrint(localizedExpression, 10000, m_evalOpts, m_printOpts);

  if (CALCULATOR->aborted()) return std::unexpected(CalculatorError("Computation aborted"));

  MathStructure const in = CALCULATOR->parse(localizedExpression);
  MathStructure const result = CALCULATOR->parse(res);
  if (result.containsUnknowns()) { return std::unexpected(CalculatorError("Unknown component in question")); }

  bool error = false;

  for (auto msg = CALCULATOR->message(); msg; msg = CALCULATOR->nextMessage()) {
    if (msg->type() == MESSAGE_ERROR) { error = true; }
  }

  if (error) return std::unexpected(CalculatorError("Calculation error"));

  CalculatorResult calcRes;
  if (result.containsType(STRUCT_UNIT)) {
    if (auto unit = getUnitDisplayName(in)) { calcRes.question.unit = Unit{.displayName = unit->c_str()}; }
    if (auto unit = getUnitDisplayName(result)) { calcRes.answer.unit = Unit{.displayName = unit->c_str()}; }
    calcRes.type = CalculatorAnswerType::CONVERSION;
  } else {
    calcRes.type = CalculatorAnswerType::NORMAL;
  }

  calcRes.question.text = question;
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

bool QalculateBackend::isComputableExpression(const std::string &expr) {
  static constexpr std::string_view opsAndParens = "~+-*/^&|!<>=() \t";
  bool allOpsOrWhitespace =
      std::ranges::all_of(expr, [](char c) { return opsAndParens.find(c) != std::string_view::npos; });
  if (allOpsOrWhitespace) return false;

  static constexpr std::string_view ops = "~+-*/^&|!<>=";
  if (std::ranges::any_of(expr, [](char c) { return ops.find(c) != std::string_view::npos; })) return true;
  if (expr.find('(') != std::string::npos) return true;
  if (expr.find(" to ") != std::string::npos) return true;

  if (auto *var = CALCULATOR->getActiveVariable(expr); var && var->isKnown()) return true;
  if (auto *fn = CALCULATOR->getActiveFunction(expr); fn && fn->minargs() == 0) return true;
  if (CALCULATOR->hasToExpression(expr, false, EvaluationOptions())) return true;

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

QFuture<QalculateBackend::ComputeResult> QalculateBackend::asyncCompute(const QString &question) {
  return QtConcurrent::run([this, question]() -> ComputeResult { return compute(question); });
}

void QalculateBackend::abort() { CALCULATOR->abort(); }

QString QalculateBackend::id() const { return "qalculate"; }

QString QalculateBackend::displayName() const { return "Qalculate!"; }

bool QalculateBackend::supportsRefreshExchangeRates() const { return true; }

QFuture<AbstractCalculatorBackend::RefreshExchangeRatesResult> QalculateBackend::refreshExchangeRates() {
  return QtConcurrent::run([this]() {
    qInfo() << "Refreshing Qalculate exchange rates...";
    auto die = [](auto &&s) {
      qWarning() << "Failed to refresh exchange rates" << s;
      return RefreshExchangeRatesResult{std::unexpected(s)};
    };
    if (!CALCULATOR->fetchExchangeRates()) { return die("Failed to fetch exchange rates"); }
    QTimer::singleShot(0, [this]() { initializeCalculator(); });
    qInfo() << "Done refreshing exchange rates";
    return RefreshExchangeRatesResult{};
  });
}

bool QalculateBackend::supportsCurrencyConversion() const { return true; }

void QalculateBackend::initializeCalculator() {
  m_evalOpts.auto_post_conversion = POST_CONVERSION_BEST;
  m_evalOpts.structuring = STRUCTURING_SIMPLIFY;
  m_evalOpts.parse_options.parsing_mode = PARSING_MODE_ADAPTIVE;
  m_evalOpts.parse_options.units_enabled = true;
  m_evalOpts.parse_options.unknowns_enabled = false;

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
  m_calc.loadGlobalDefinitions();
  m_calc.loadLocalDefinitions();
  m_calc.loadExchangeRates();
  m_calc.loadGlobalCurrencies();
  m_calc.loadGlobalUnits();
  m_calc.loadGlobalVariables();
  m_calc.loadGlobalFunctions();

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
