#pragma once
#include <expected>
#include "vicinae.hpp"

class ScriptMetadataStore {
  struct RunMetadata {
    std::int64_t lastRunAt;
    std::string output; // encoded as base64 to make sure we always serialize valid json
  };

public:
  ScriptMetadataStore(const std::filesystem::path &path = Omnicast::dataDir() / "script-metadata.json");

  void saveRun(std::string_view scriptId, std::string_view line);
  std::optional<std::string> lastRunData(const std::string &id) const;

private:
  struct Data {
    std::unordered_map<std::string, RunMetadata> inlineRuns;
  };

  std::optional<std::string> syncWithDisk();
  std::expected<Data, std::string> loadDataFromDisk();

  Data m_data;
  std::filesystem::path m_path;
};
