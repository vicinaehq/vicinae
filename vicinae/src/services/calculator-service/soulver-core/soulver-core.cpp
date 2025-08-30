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

SoulverCoreCalculator::SoulverCoreCalculator() {
  m_dlHandle = dlopen("libSoulverWrapper.so", RTLD_LAZY);

  if (!m_dlHandle) {
    qDebug() << "unable to load libSoulverWrapper" << dlerror();
  } else {
    loadABI();
    qDebug() << "loaded ABI for libSoulverWrapper";
  }
}

bool SoulverCoreCalculator::isActivatable() const { return m_dlHandle && !availableResourcePaths().empty(); };

void SoulverCoreCalculator::start() {
  for (const auto &path : availableResourcePaths()) {
    m_abi.soulver_initialize(path.c_str());

    if (m_abi.soulver_is_initialized()) break;
  }

  if (!m_abi.soulver_is_initialized()) { qCritical() << "SoulverCore calculator could not be initialized"; }
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
  char *answer = m_abi.soulver_evaluate(question.toStdString().c_str());

  if (!answer) {
    qWarning() << "soulver_evaluate returned a null pointer. This suggests soulver crashed or wasn't "
                  "properly initialized.";
    return std::unexpected(CalculatorError("Failed to parse json"));
  }

  qDebug() << answer;

  QJsonParseError parseError;
  auto doc = QJsonDocument::fromJson(answer, &parseError);

  free(answer);

  if (parseError.error != QJsonParseError::NoError) {
    return std::unexpected(CalculatorError("Failed to parse json"));
  }

  auto json = doc.object();

  CalculatorResult result;
  QString value = json.value("value").toString();
  QString type = json.value("type").toString();

  if (type == "none") { return std::unexpected(CalculatorError("Result is of none type")); }

  result.question = question;
  result.answer = value;
  result.type = CalculatorAnswerType::NORMAL;

  return result;
};
