#pragma once

#include "services/ai/ai-tool.hpp"

namespace config {
class Manager;
}

class ReadConfigTool : public AI::AbstractTool {
public:
  explicit ReadConfigTool(const config::Manager &config) : m_config(config) {}

  std::string name() const override { return "read_config"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  const config::Manager &m_config;
};

class WriteConfigTool : public AI::AbstractTool {
public:
  explicit WriteConfigTool(config::Manager &config) : m_config(config) {}

  std::string name() const override { return "write_config"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  config::Manager &m_config;
};
