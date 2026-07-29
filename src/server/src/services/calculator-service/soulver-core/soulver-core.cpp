#include "soulver-core.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <filesystem>
#include <QDebug>
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

  result.question.text = question;
  result.answer.text = soulverRes.value().result;
  result.type = CalculatorAnswerType::NORMAL;

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
  const CPtr<char> answer(m_abi.soulver_evaluate(expression.toStdString().c_str()));

  if (!answer) {
    qWarning() << "soulver_evaluate returned a null pointer. This suggests soulver crashed or wasn't "
                  "properly initialized.";
    return std::unexpected("Failed to parse json");
  }

  QJsonParseError parseError;
  auto doc = QJsonDocument::fromJson(answer.get(), &parseError);

  if (parseError.error != QJsonParseError::NoError) { return std::unexpected("Failed to parse json"); }

  auto json = doc.object();
  QString const value = json.value("value").toString();
  QString const type = json.value("type").toString();

  SoulverResult result;

  result.result = json.value("value").toString();
  result.type = json.value("type").toString();

  if (json.contains("error")) { result.error = json.value("error").toString(); }

  return result;
}
