#include "soulver-core.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <filesystem>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "common/c-ptr.hpp"

#ifndef Q_OS_MACOS
#include "xdgpp/env/env.hpp"
#include <dlfcn.h>
#endif

namespace fs = std::filesystem;

#ifdef Q_OS_MACOS

extern "C" {
bool soulver_initialize(const char *resourcesPath);
bool soulver_is_initialized(void);
char *soulver_evaluate(const char *expression);
}

SoulverCoreCalculator::SoulverCoreCalculator() {
  m_abi.soulver_initialize = &::soulver_initialize;
  m_abi.soulver_is_initialized = &::soulver_is_initialized;
  m_abi.soulver_evaluate = &::soulver_evaluate;
}

bool SoulverCoreCalculator::isActivatable() const { return true; };

bool SoulverCoreCalculator::start() {
  m_abi.soulver_initialize("");

  auto test = calculate("2+2");
  bool const canCompute = test.has_value() && test.value().result == "4";

  return canCompute;
}

#else

constexpr const char *SHARED_LIB = "libSoulverWrapper.so";

SoulverCoreCalculator::SoulverCoreCalculator() : m_dlHandle(dlopen(SHARED_LIB, RTLD_LAZY)) {

  if (!m_dlHandle) {
    qDebug() << "unable to load libSoulverWrapper" << dlerror();
  } else {
    loadABI();
  }
}

bool SoulverCoreCalculator::isActivatable() const { return m_dlHandle && !availableResourcePaths().empty(); };

bool SoulverCoreCalculator::start() {
  for (const auto &path : availableResourcePaths()) {
    if (m_abi.soulver_initialize(path.c_str())) break;
  }

  auto test = calculate("2+2");
  bool const canCompute = test.has_value() && test.value().result == "4";

  return canCompute;
}

void SoulverCoreCalculator::loadABI() {
  m_abi.soulver_initialize =
      reinterpret_cast<bool (*)(const char *)>(dlsym(m_dlHandle, "soulver_initialize"));
  m_abi.soulver_is_initialized =
      reinterpret_cast<bool (*)(void)>(dlsym(m_dlHandle, "soulver_is_initialized"));
  m_abi.soulver_evaluate = reinterpret_cast<char *(*)(const char *)>(dlsym(m_dlHandle, "soulver_evaluate"));
  m_abi.soulver_evaluate_ex =
      reinterpret_cast<char *(*)(const char *)>(dlsym(m_dlHandle, "soulver_evaluate_ex"));
}

std::vector<fs::path> SoulverCoreCalculator::availableResourcePaths() const {
  std::error_code ec;
  std::vector<fs::path> paths;

  for (const auto &dir : xdgpp::dataDirs()) {
    fs::path const resource = dir / "soulver-core" / "resources";
    if (fs::is_directory(resource, ec)) { paths.emplace_back(resource); }
  }

  return paths;
}

#endif

std::expected<AbstractCalculatorBackend::CalculatorResult, AbstractCalculatorBackend::CalculatorError>
SoulverCoreCalculator::compute(const QString &question, const ComputeOptions &opts) {
  const auto fail = [](auto &&reason) { return std::unexpected(CalculatorError{reason}); };

  auto soulverRes = calculate(question);

  if (!soulverRes) return fail(soulverRes.error());
  if (soulverRes.value().type == "none") return fail("Result type is none");

  CalculatorResult result;
  const auto candidateResult = [](const SoulverResult::Alternative &candidate) {
    CalculatorResult alternative;
    alternative.question.text = candidate.title;
    alternative.answer.text = candidate.result;
    alternative.answer.subtitle = candidate.subtitle;
    alternative.type = CalculatorAnswerType::NORMAL;
    return alternative;
  };

  if (soulverRes.value().alternatives.empty()) {
    result.question.text = question;
    result.answer.text = soulverRes.value().result;
    result.answer.subtitle = soulverRes.value().subtitle;
    result.type = CalculatorAnswerType::NORMAL;
  } else {
    result = candidateResult(soulverRes.value().alternatives.front());
    result.alternatives.reserve(soulverRes.value().alternatives.size() - 1);
    for (const auto &candidate : soulverRes.value().alternatives | std::views::drop(1)) {
      result.alternatives.emplace_back(candidateResult(candidate));
    }
  }

  return result;
};

QFuture<SoulverCoreCalculator::ComputeResult>
SoulverCoreCalculator::asyncCompute(const QString &question, const ComputeOptions &opts) {
  QPromise<ComputeResult> promise;
  promise.addResult(compute(question, opts));
  promise.finish();
  return promise.future();
}

std::expected<SoulverCoreCalculator::SoulverResult, QString>
SoulverCoreCalculator::calculate(const QString &expression) const {
  const auto evaluate = m_abi.soulver_evaluate_ex ? m_abi.soulver_evaluate_ex : m_abi.soulver_evaluate;
  const CPtr<char> answer(evaluate(expression.toStdString().c_str()));

  if (!answer) {
    qWarning() << (m_abi.soulver_evaluate_ex ? "soulver_evaluate_ex" : "soulver_evaluate")
               << "returned a null pointer. This suggests soulver crashed or wasn't properly initialized.";
    return std::unexpected("Failed to parse json");
  }

  QJsonParseError parseError;
  auto doc = QJsonDocument::fromJson(answer.get(), &parseError);

  if (parseError.error != QJsonParseError::NoError) { return std::unexpected("Failed to parse json"); }

  auto json = doc.object();
  SoulverResult result;

  result.result = json.value("value").toString();
  result.type = json.value("type").toString();
  if (const auto subtitle = json.value("subtitle"); subtitle.isString()) {
    result.subtitle = subtitle.toString();
  }

  const auto candidates = json.value("candidates").toArray();
  result.alternatives.reserve(candidates.size());
  for (const auto &value : candidates) {
    const auto candidate = value.toObject();
    const auto title = candidate.value("title");
    const auto answer = candidate.value("value");
    if (!title.isString() || !answer.isString()) continue;

    SoulverResult::Alternative alternative{
        .title = title.toString(),
        .result = answer.toString(),
    };
    if (const auto subtitle = candidate.value("subtitle"); subtitle.isString()) {
      alternative.subtitle = subtitle.toString();
    }
    result.alternatives.emplace_back(std::move(alternative));
  }

  if (json.contains("error")) { result.error = json.value("error").toString(); }

  return result;
}
