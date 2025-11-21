#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QDebug>
#include <format>
#include <libqalculate/MathStructure.h>
#include <libqalculate/QalculateDateTime.h>
#include <libqalculate/includes.h>
#include <libqalculate/Unit.h>
#include <libqalculate/Prefix.h>
#include <qlogging.h>
#include <QtConcurrent/QtConcurrent>
#include <qobjectdefs.h>

using CalculatorResult = QalculateBackend::CalculatorResult;
using CalculatorError = QalculateBackend::CalculatorError;

QalculateBackend::QalculateBackend() {}

bool QalculateBackend::isActivatable() const {
  /**
   * if QalculateBackend is considered as an option, it means support for it was compiled in
   * and the vicinae binary links to the qalculate library already. That's why we don't need complex checks.
   */
  return true;
}

bool QalculateBackend::start() {
  if (!m_initialized) {
    initializeCalculator();
    m_initialized = true;
  }

  return true;
}

tl::expected<CalculatorResult, CalculatorError> QalculateBackend::compute(const QString &question) const {
  QString expression = preprocessQuestion(question);

  MathStructure out;
  MathStructure in;
  MathStructure result = CALCULATOR->calculate(CALCULATOR->unlocalizeExpression(expression.toStdString()),
                                               m_evalOpts, &in, &out);

  if (result.containsUnknowns()) { return tl::unexpected(CalculatorError("Unknown component in question")); }

  bool error = false;

  for (auto msg = CALCULATOR->message(); msg; msg = CALCULATOR->nextMessage()) {
    error = true;
  }

  if (error) return tl::unexpected(CalculatorError("Calculation error"));

  std::string res = result.print(m_printOpts);
  CalculatorResult calcRes;

  if (auto unit = getUnitDisplayName(in)) { calcRes.question.unit = Unit{.displayName = unit->c_str()}; }
  if (auto unit = getUnitDisplayName(result)) { calcRes.answer.unit = Unit{.displayName = unit->c_str()}; }

  calcRes.question.text = question;
  calcRes.answer.text = QString::fromStdString(res);

  if (result.containsType(STRUCT_UNIT)) {
    calcRes.type = CalculatorAnswerType::CONVERSION;
  } else {
    calcRes.type = CalculatorAnswerType::NORMAL;
  }

  return calcRes;
}

QString QalculateBackend::preprocessQuestion(const QString &query) const { return query.simplified(); }

QFuture<QalculateBackend::ComputeResult> QalculateBackend::asyncCompute(const QString &question) const {
  QPromise<ComputeResult> promise;
  promise.addResult(compute(question));
  promise.finish();
  return promise.future();
}

QString QalculateBackend::id() const { return "qalculate"; }

QString QalculateBackend::displayName() const { return "Qalculate!"; }

bool QalculateBackend::supportsRefreshExchangeRates() const { return true; }

QFuture<AbstractCalculatorBackend::RefreshExchangeRatesResult> QalculateBackend::refreshExchangeRates() {
  return QtConcurrent::run([this]() {
    qInfo() << "Refreshing Qalculate exchange rates...";
    auto die = [](auto &&s) {
      qWarning() << "Failed to refresh exchange rates" << s;
      return RefreshExchangeRatesResult{tl::unexpected(s)};
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
  m_evalOpts.parse_options.limit_implicit_multiplication = true;
  m_evalOpts.parse_options.parsing_mode = PARSING_MODE_CONVENTIONAL;
  m_evalOpts.parse_options.units_enabled = true;
  m_evalOpts.parse_options.unknowns_enabled = false;

  m_printOpts.indicate_infinite_series = true;
  m_printOpts.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
  m_printOpts.use_unicode_signs = true;

  m_calc.reset();
  m_calc.loadGlobalDefinitions();
  m_calc.loadLocalDefinitions();
  m_calc.loadExchangeRates();
  m_calc.loadGlobalCurrencies();
  m_calc.loadGlobalUnits();
  m_calc.loadGlobalVariables();
  m_calc.loadGlobalFunctions();
}

std::optional<std::string> QalculateBackend::getUnitDisplayName(const MathStructure &s,
                                                                std::string_view prefix) {
  if (prefix.empty() && s.prefix()) {
    std::string_view prefix = s.prefix()->preferredDisplayName().name;
    if (!prefix.empty()) { return getUnitDisplayName(s, prefix); }
  }

  if (auto unit = s.unit()) {
    return std::format("{}{}", prefix, unit->preferredDisplayName(false, false, true, false).name);
  }

  for (int i = 0; i != s.size(); ++i) {
    if (auto unit = getUnitDisplayName(s[i], prefix)) { return unit; }
  }

  return std::nullopt;
}
