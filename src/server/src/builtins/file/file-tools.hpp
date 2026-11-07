#pragma once

#include "services/ai/ai-tool.hpp"

class FileService;

class SearchFilesTool : public AI::AbstractTool {
public:
  explicit SearchFilesTool(FileService &files) : m_files(files) {}

  std::string name() const override { return "search"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  bool isEnabled() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;

private:
  FileService &m_files;
};

class ReadFileTool : public AI::AbstractTool {
public:
  std::string name() const override { return "read"; }
  std::string description() const override;
  std::string generateInputSchema() const override;
  std::optional<std::string> invocationSummary(std::string_view arguments) const override;
  RawToolTask runRaw(std::string_view arguments) override;
};
