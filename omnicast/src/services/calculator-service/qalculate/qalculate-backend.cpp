#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include <QDebug>
#include <libqalculate/MathStructure.h>
#include <libqalculate/QalculateDateTime.h>
#include <libqalculate/includes.h>
#include <qlogging.h>

using CalculatorResult = QalculateBackend::CalculatorResult;
using CalculatorError = QalculateBackend::CalculatorError;

std::expected<CalculatorResult, CalculatorError> QalculateBackend::compute(const QString &question) const {
  EvaluationOptions evalOpts;

  evalOpts.auto_post_conversion = POST_CONVERSION_BEST;
  evalOpts.structuring = STRUCTURING_SIMPLIFY;
  evalOpts.parse_options.limit_implicit_multiplication = true;
  evalOpts.parse_options.parsing_mode = PARSING_MODE_CONVENTIONAL;
  evalOpts.parse_options.units_enabled = true;
  evalOpts.parse_options.unknowns_enabled = false;

  MathStructure result = CALCULATOR->calculate(question.toStdString(), evalOpts);

  if (result.containsUnknowns()) { return std::unexpected(CalculatorError("Unknown component in question")); }

  bool error = false;

  for (auto msg = CALCULATOR->message(); msg; msg = CALCULATOR->nextMessage()) {
    qCritical() << "Calculator Error" << msg->message();
    error = true;
  }

  if (error) return std::unexpected(CalculatorError("Calculation error"));

  PrintOptions printOpts;

  printOpts.indicate_infinite_series = true;
  printOpts.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;
  printOpts.use_unicode_signs = true;

  std::string res = result.print(printOpts);

  for (int i = 0; i != result.size(); ++i) {
    if (result[i].isUnit()) { qCritical() << "UNIT INVOLVED!"; }
  }

  CalculatorResult calcRes;

  calcRes.question = question;
  calcRes.answer = QString::fromStdString(res);

  if (result.containsType(STRUCT_UNIT)) {
    qDebug() << "Has unit";
    calcRes.type = CalculatorAnswerType::CONVERSION;
  } else {
    calcRes.type = CalculatorAnswerType::NORMAL;
  }

  return calcRes;
}

QString QalculateBackend::name() const { return "qalculate"; }

bool QalculateBackend::reloadExchangeRates() const {
  CALCULATOR->fetchExchangeRates();
  return false;
}

bool QalculateBackend::supportsCurrencyConversion() const { return true; }

QalculateBackend::QalculateBackend() {
  m_calc.checkExchangeRatesDate(1);
  m_calc.loadExchangeRates();
  m_calc.loadGlobalDefinitions();
  m_calc.loadLocalDefinitions();

  size_t idx = 1;
  std::string url;

  while (true) {
    url = m_calc.getExchangeRatesUrl(idx++);
    if (url.empty()) break;
    qDebug() << "exchange rate url" << url;
  }
}
