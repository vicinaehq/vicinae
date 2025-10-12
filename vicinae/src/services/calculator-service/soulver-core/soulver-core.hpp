#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "utils/expected.hpp"
#include <filesystem>

class SoulverCoreCalculator : public AbstractCalculatorBackend {

public:
  struct SoulverResult {
    QString type;
    QString result;
    std::optional<QString> error;
  };

  bool isActivatable() const override;
  bool start() override;
  ComputeResult compute(const QString &question) const override;
  QFuture<ComputeResult> asyncCompute(const QString &question) const override;

  QString displayName() const override { return "SoulverCore"; }
  QString id() const override { return "soulver-core"; }

  SoulverCoreCalculator();

private:
  struct ABI {
    bool (*soulver_initialize)(const char *) = nullptr;
    bool (*soulver_is_initialized)(void) = nullptr;
    char *(*soulver_evaluate)(const char *) = nullptr;
  };

  void loadABI();

  tl::expected<SoulverResult, QString> calculate(const QString &expression) const;
  std::vector<std::filesystem::path> availableResourcePaths() const;
  ABI m_abi;

  void *m_dlHandle = nullptr;
};
