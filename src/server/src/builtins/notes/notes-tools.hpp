#pragma once

#include "services/ai/ai-tool.hpp"

namespace Notes {
class Store;
}

class SearchNotesTool : public AI::AbstractTool {
public:
  explicit SearchNotesTool(Notes::Store &store) : m_store(store) {}

  std::string name() const override { return "search"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  Notes::Store &m_store;
};

class ReadNoteTool : public AI::AbstractTool {
public:
  explicit ReadNoteTool(Notes::Store &store) : m_store(store) {}

  std::string name() const override { return "read"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  Notes::Store &m_store;
};

class WriteNoteTool : public AI::AbstractTool {
public:
  explicit WriteNoteTool(Notes::Store &store) : m_store(store) {}

  std::string name() const override { return "write"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  bool requiresApproval(std::string_view arguments) const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  Notes::Store &m_store;
};
