#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <QtGlobal>
#include <expected>
#include <filesystem>

class SoulverCoreCalculator : public AbstractCalculatorBackend {

public:
  struct SoulverResult {
    struct Alternative {
      QString title;
      QString result;
      std::optional<QString> subtitle;
    };

    QString type;
    QString result;
    std::optional<QString> subtitle;
    std::vector<Alternative> alternatives;
    std::optional<QString> error;
  };

  bool isActivatable() const override;
  bool start() override;
  ComputeResult compute(const QString &question, const ComputeOptions &opts) override;
  QFuture<ComputeResult> asyncCompute(const QString &question, const ComputeOptions &opts) override;

  QString displayName() const override { return "SoulverCore"; }
  QString id() const override { return "soulver-core"; }

  SoulverCoreCalculator();

private:
  struct ABI {
    bool (*soulver_initialize)(const char *) = nullptr;
    bool (*soulver_is_initialized)(void) = nullptr;
    char *(*soulver_evaluate)(const char *) = nullptr;
    char *(*soulver_evaluate_ex)(const char *) = nullptr;
  };

  std::expected<SoulverResult, QString> calculate(const QString &expression) const;
  ABI m_abi;

#ifndef Q_OS_MACOS
  void loadABI();
  std::vector<std::filesystem::path> availableResourcePaths() const;

  void *m_dlHandle = nullptr;
#endif
};
