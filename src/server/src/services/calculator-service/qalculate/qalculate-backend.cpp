#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QDebug>
#include <format>
#include <libqalculate/MathStructure.h>
#include <libqalculate/QalculateDateTime.h>
#include <libqalculate/includes.h>
#include <libqalculate/Unit.h>
#include <libqalculate/Prefix.h>
#include <optional>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <QtConcurrent/QtConcurrent>
#include <qnamespace.h>
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

std::expected<CalculatorResult, CalculatorError> QalculateBackend::compute(const QString &question) const {
  QString expression = preprocessQuestion(question);

  EvaluationOptions evalOpts = m_evalOpts;
  PrintOptions printOpts = m_printOpts;

  std::string localizedExpression = CALCULATOR->unlocalizeExpression(expression.toStdString());
  std::string res = CALCULATOR->calculateAndPrint(localizedExpression, 10000, m_evalOpts, m_printOpts);
  MathStructure in = CALCULATOR->parse(localizedExpression);
  MathStructure result = CALCULATOR->parse(res);

  if (auto err = drainMessages())
    return std::unexpected(CalculatorError(err.value()));

  CalculatorResult calcRes;
  calcRes.question.text = question;
  calcRes.answer.text = QString::fromStdString(res);

  if (result.containsType(STRUCT_UNIT) && !isIndefiniteIntegral(expression)) {
    if (auto unit = getUnitDisplayName(in)) { calcRes.question.unit = Unit{.displayName = unit->c_str()}; }
    if (auto unit = getUnitDisplayName(result)) { calcRes.answer.unit = Unit{.displayName = unit->c_str()}; }
    calcRes.type = CalculatorAnswerType::CONVERSION;
  } else {
    calcRes.type = CalculatorAnswerType::NORMAL;
  }

  return calcRes;
}

std::optional<QString> QalculateBackend::drainMessages() {
  std::optional<QString> firstError;
  for (auto *msg = CALCULATOR->message(); msg; msg = CALCULATOR->nextMessage()) {
    switch (msg->type()) {
      case MESSAGE_ERROR:
        if (!firstError) firstError = QString::fromStdString(msg->message());
        qWarning() << "[Qalculate] error:"   << msg->message().c_str();
        break;
      case MESSAGE_WARNING:
        qWarning() << "[Qalculate] warning:" << msg->message().c_str();
        break;
      default:
        qDebug()   << "[Qalculate] info:"    << msg->message().c_str();
        break;
    }
  }
  return firstError;
}

// I don't really like regex bashing to determine this but I couldn't
// find any cleaner way to do it so I guess this works for now
bool QalculateBackend::isIndefiniteIntegral(const QString &expr) {
    const QString e = expr.simplified().toLower();

    static const QRegularExpression integrateCall(
        R"(\b(integrate|integral)\s*\()"
    );

    static const QRegularExpression integratePrefixOnly(
        R"(\b(integrate|integral)\b(?!\s*\())"
    );
    if (integratePrefixOnly.match(e).hasMatch())
        return true;

    if (!integrateCall.match(e).hasMatch())
        return false;

    static const QRegularExpression outerArgs(
        R"(\b(?:integrate|integral)\s*\((.+)\))"
    );
    const auto m = outerArgs.match(e);
    if (!m.hasMatch())
        return true;

    // regex fails bc you can do smtn like "integral(f(x, y), y)" which should be indefinite
    const QString args = m.captured(1);
    int depth = 0, argCount = 1;
    for (const QChar c : args) {
        if (c == '(' || c == '[') ++depth;
        else if (c == ')' || c == ']') --depth;
        else if (c == ',' && depth == 0) ++argCount;
    }

    return argCount < 3;
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
  m_evalOpts = EvaluationOptions{};
  m_evalOpts.auto_post_conversion = POST_CONVERSION_NONE;
  m_evalOpts.auto_post_conversion = POST_CONVERSION_OPTIMAL;
  m_evalOpts.do_polynomial_division = true;
  m_evalOpts.structuring = STRUCTURING_SIMPLIFY;
  m_evalOpts.parse_options.angle_unit = ANGLE_UNIT_DEGREES;
  m_evalOpts.parse_options.limit_implicit_multiplication = false;
  m_evalOpts.parse_options.parsing_mode = PARSING_MODE_ADAPTIVE;
  m_evalOpts.parse_options.read_precision = DONT_READ_PRECISION;

  m_printOpts = PrintOptions{};
  m_printOpts.base_display = BASE_DISPLAY_ALTERNATIVE;
  m_printOpts.number_fraction_format = FRACTION_DECIMAL_EXACT;
  m_printOpts.limit_implicit_multiplication = false;
  m_printOpts.spacious = false;
  m_printOpts.allow_factorization = true;
  m_printOpts.interval_display = INTERVAL_DISPLAY_PLUSMINUS;
  m_printOpts.use_unicode_signs = true;
  m_printOpts.time_zone = TIME_ZONE_LOCAL;

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
