#pragma once

#include <optional>
#include <QCoreApplication>
#include <QString>
#include "services/ai/ai-tool.hpp"

class BashTool : public AI::AbstractTool {
  Q_DECLARE_TR_FUNCTIONS(BashTool)

public:
  explicit BashTool(QString executable) : m_executable(std::move(executable)) {}

  static std::optional<QString> executablePath();
  std::string name() const override { return "bash"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  QString m_executable;
};
