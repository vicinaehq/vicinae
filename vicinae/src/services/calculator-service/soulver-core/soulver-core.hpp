#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <expected>
#include <filesystem>

class SoulverCoreCalculator : public AbstractCalculatorBackend {
public:
  bool isActivatable() const override;
  void start() override;
  std::expected<CalculatorResult, CalculatorError> compute(const QString &question) const override;

  QString displayName() const override { return "SoulverCore"; }
  QString id() const override { return "soulver-core"; }

  SoulverCoreCalculator();

private:
  struct ABI {
    void (*soulver_initialize)(const char *) = nullptr;
    char *(*soulver_evaluate)(const char *) = nullptr;
    bool (*soulver_is_initialized)(void) = nullptr;
  };

  void loadABI();

  std::vector<std::filesystem::path> availableResourcePaths() const;
  ABI m_abi;

  void *m_dlHandle = nullptr;
};
; // namespace SoulverCore
