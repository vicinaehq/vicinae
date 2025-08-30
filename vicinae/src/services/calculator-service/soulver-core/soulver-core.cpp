#include "soulver-core.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "vicinae.hpp"
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <qjsonparseerror.h>
#include <qlogging.h>
#include <ranges>

namespace fs = std::filesystem;

constexpr const char *SHARED_LIB = "libSoulverWrapper.so";

SoulverCoreCalculator::SoulverCoreCalculator() {
  m_dlHandle = dlopen(SHARED_LIB, RTLD_LAZY);

  if (!m_dlHandle) {
    qDebug() << "unable to load libSoulverWrapper" << dlerror();
  } else {
    loadABI();
  }
}

bool SoulverCoreCalculator::isActivatable() const { return m_dlHandle && !availableResourcePaths().empty(); };

bool SoulverCoreCalculator::start() {
  for (const auto &path : availableResourcePaths()) {
    m_abi.soulver_initialize(path.c_str());

    if (m_abi.soulver_is_initialized()) break;
  }

  auto test = calculate("2+2");
  bool canCompute = test.has_value() && test.value().result == "4";

  return canCompute;
}

void SoulverCoreCalculator::loadABI() {
  m_abi.soulver_initialize =
      reinterpret_cast<void (*)(const char *)>(dlsym(m_dlHandle, "soulver_initialize"));
  m_abi.soulver_is_initialized =
      reinterpret_cast<bool (*)(void)>(dlsym(m_dlHandle, "soulver_is_initialized"));
  m_abi.soulver_evaluate = reinterpret_cast<char *(*)(const char *)>(dlsym(m_dlHandle, "soulver_evaluate"));
}

std::vector<fs::path> SoulverCoreCalculator::availableResourcePaths() const {
  auto toResource = [](const fs::path &path) { return path / "soulver-cpp" / "resources"; };
  auto isValidResourceDir = [](const fs::path &path) {
    std::error_code ec;
    return fs::is_directory(path, ec);
  };

  return Omnicast::xdgDataDirs() | std::views::transform(toResource) |
         std::views::filter(isValidResourceDir) | std::ranges::to<std::vector>();
}

std::expected<AbstractCalculatorBackend::CalculatorResult, AbstractCalculatorBackend::CalculatorError>
SoulverCoreCalculator::compute(const QString &question) const {
  auto soulverRes = calculate(question);

  if (!soulverRes) { return std::unexpected(CalculatorError(soulverRes.error())); }
  if (soulverRes.value().type == "none") return std::unexpected(CalculatorError("Result type is none"));

  CalculatorResult result;

  result.question = question;
  result.answer = soulverRes.value().result;
  result.type = CalculatorAnswerType::NORMAL;

  return result;
};

std::expected<SoulverCoreCalculator::SoulverResult, QString>
SoulverCoreCalculator::calculate(const QString &expression) const {
  char *answer = m_abi.soulver_evaluate(expression.toStdString().c_str());

  if (!answer) {
    qWarning() << "soulver_evaluate returned a null pointer. This suggests soulver crashed or wasn't "
                  "properly initialized.";
    return std::unexpected("Failed to parse json");
  }

  qInfo() << answer;

  QJsonParseError parseError;
  auto doc = QJsonDocument::fromJson(answer, &parseError);

  free(answer);

  if (parseError.error != QJsonParseError::NoError) { return std::unexpected("Failed to parse json"); }

  auto json = doc.object();
  QString value = json.value("value").toString();
  QString type = json.value("type").toString();

  SoulverResult result;

  result.result = json.value("value").toString();
  result.type = json.value("type").toString();

  if (json.contains("error")) { result.error = json.value("error").toString(); }

  return result;
}
