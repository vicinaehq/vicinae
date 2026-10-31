#pragma once

#include "services/ai/ai-tool.hpp"

class AppService;

class OpenTool : public AI::AbstractTool {
public:
  explicit OpenTool(const AppService &apps) : m_apps(apps) {}

  std::string name() const override { return "open"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  const AppService &m_apps;
};
