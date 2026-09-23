#pragma once

#include "ai-tool.hpp"

namespace AI {

class BashTool : public AbstractTool {
public:
  static bool available();
  std::string name() const override { return "bash"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;
};

} // namespace AI
