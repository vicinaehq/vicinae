#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QDebug>
#include <libqalculate/MathStructure.h>
#include <libqalculate/QalculateDateTime.h>
#include <libqalculate/includes.h>
#include <qlogging.h>
#include <QtConcurrent/QtConcurrent>
#include <stdexcept>

using CalculatorResult = QalculateBackend::CalculatorResult;
using CalculatorError = QalculateBackend::CalculatorError;

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

void QalculateBackend::initializeCalculator() {
  m_calc.reset();
  m_calc.loadGlobalDefinitions();
  m_calc.loadLocalDefinitions();
  m_calc.loadExchangeRates();
  m_calc.loadGlobalCurrencies();
  m_calc.loadGlobalUnits();
  m_calc.loadGlobalVariables();
  m_calc.loadGlobalFunctions();
}

tl::expected<CalculatorResult, CalculatorError> QalculateBackend::compute(const QString &question) const {
  EvaluationOptions evalOpts;

  evalOpts.auto_post_conversion = POST_CONVERSION_BEST;
  evalOpts.structuring = STRUCTURING_SIMPLIFY;
  evalOpts.parse_options.limit_implicit_multiplication = true;
  evalOpts.parse_options.parsing_mode = PARSING_MODE_CONVENTIONAL;
  evalOpts.parse_options.units_enabled = true;
  evalOpts.parse_options.unknowns_enabled = false;

  MathStructure result = CALCULATOR->calculate(question.toStdString(), evalOpts);

  if (result.containsUnknowns()) { return tl::unexpected(CalculatorError("Unknown component in question")); }

  bool error = false;

  for (auto msg = CALCULATOR->message(); msg; msg = CALCULATOR->nextMessage()) {
    error = true;
  }

  if (error) return tl::unexpected(CalculatorError("Calculation error"));

  PrintOptions printOpts;

  printOpts.indicate_infinite_series = true;
  printOpts.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
  printOpts.use_unicode_signs = true;

  std::string res = result.print(printOpts);

  CalculatorResult calcRes;

  calcRes.question = question;
  calcRes.answer = QString::fromStdString(res);

  if (result.containsType(STRUCT_UNIT)) {
    calcRes.type = CalculatorAnswerType::CONVERSION;
  } else {
    calcRes.type = CalculatorAnswerType::NORMAL;
  }

  return calcRes;
}

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

QalculateBackend::QalculateBackend() {}
