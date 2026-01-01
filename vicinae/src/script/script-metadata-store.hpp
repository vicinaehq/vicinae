#pragma once
#include <expected>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/key_transformers.hpp>
#include "utils.hpp"
#include "vicinae.hpp"

class ScriptMetadataStore {
  struct RunMetadata {
    std::int64_t lastRunAt;
    std::string output; // encoded as base64 to make sure we always serialize valid json
  };

  struct Data {
    std::unordered_map<std::string, RunMetadata> inlineRuns;
  };

public:
  ScriptMetadataStore(const std::filesystem::path &path = Omnicast::dataDir() / "script-metadata.json")
      : m_path(path) {
    std::error_code ec;

    if (!std::filesystem::is_regular_file(path, ec)) { syncWithDisk(); }

    auto result = loadDataFromDisk();

    if (!result) {
      qWarning() << "Failed to load script metadata from" << m_path << result.error();
      return;
    }

    m_data = *result;
  }

  void saveRun(std::string_view scriptId, std::string_view line) {
    m_data.inlineRuns[std::string{scriptId}] =
        RunMetadata{.lastRunAt = QDateTime::currentSecsSinceEpoch(), .output = Utils::toBase64(line)};

    syncWithDisk();
  }

  std::optional<std::string> lastRunData(const std::string &id) const {
    if (const auto it = m_data.inlineRuns.find(id); it != m_data.inlineRuns.end()) {
      return Utils::fromBase64(it->second.output);
    }

    return {};
  }

private:
  std::optional<std::string> syncWithDisk() {
    std::string buf;

    if (const auto error = glz::write_file_json(m_data, m_path.c_str(), buf)) {
      return glz::format_error(error);
    }

    return {};
  }

  std::expected<Data, std::string> loadDataFromDisk() {
    Data data;
    std::string buf;
    if (const auto error = glz::read_file_json(data, m_path.c_str(), buf)) {
      return std::unexpected(glz::format_error(error));
    }
    return data;
  }

  Data m_data;
  std::filesystem::path m_path;
};

template <> struct glz::meta<ScriptMetadataStore::Data> : glz::snake_case {};
template <> struct glz::meta<ScriptMetadataStore::RunMetadata> : glz::snake_case {};
