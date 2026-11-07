#pragma once

#include "services/ai/ai-tool.hpp"

namespace config {
class Manager;
}

class ReadThemesTool : public AI::AbstractTool {
public:
  explicit ReadThemesTool(const config::Manager &config) : m_config(config) {}

  std::string name() const override { return "read_themes"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  const config::Manager &m_config;
};

class WriteThemeTool : public AI::AbstractTool {
public:
  std::string name() const override { return "write_theme"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  bool requiresApproval(std::string_view arguments) const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;
};

class SetThemeTool : public AI::AbstractTool {
public:
  explicit SetThemeTool(config::Manager &config) : m_config(config) {}

  std::string name() const override { return "set_theme"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  config::Manager &m_config;
};
