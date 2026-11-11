#pragma once

#include <string>
#include <string_view>
#include "services/ai/ai-tool.hpp"

namespace AI {

constexpr std::string_view WEB_SEARCH_TOOL_ID = "web_search";

class WebSearchTool final : public AbstractTool {
public:
  std::string name() const override { return std::string(WEB_SEARCH_TOOL_ID); }
  std::string description() const override {
    return "Search the web for current information. Use this when the user asks about recent or online "
           "information.";
  }
  std::string generateInputSchema() const override {
    return R"({"type":"object","properties":{"query":{"type":"string","description":"The web search query."}},"required":["query"]})";
  }
  RawToolTask runRaw(std::string_view arguments) override;
  bool isEnabled() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
};

} // namespace AI
