#pragma once

#include "services/ai/ai-tool.hpp"

class ClipboardService;

class SearchClipboardTool : public AI::AbstractTool {
public:
  explicit SearchClipboardTool(const ClipboardService &clipboard) : m_clipboard(clipboard) {}

  std::string name() const override { return "search_history"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  const ClipboardService &m_clipboard;
};

class ReadClipboardEntryTool : public AI::AbstractTool {
public:
  explicit ReadClipboardEntryTool(const ClipboardService &clipboard) : m_clipboard(clipboard) {}

  std::string name() const override { return "read_entry"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  const ClipboardService &m_clipboard;
};
