#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/key_transformers.hpp>
#include "script-metadata-store.hpp"
#include "utils.hpp"

ScriptMetadataStore::ScriptMetadataStore(const std::filesystem::path &path) : m_path(path) {
  std::error_code ec;

  if (!std::filesystem::is_regular_file(path, ec)) { syncWithDisk(); }

  auto result = loadDataFromDisk();

  if (!result) {
    qWarning() << "Failed to load script metadata from" << m_path << result.error();
    return;
  }

  m_data = *result;
}

void ScriptMetadataStore::saveRun(std::string_view scriptId, std::string_view line) {
  m_data.inlineRuns[std::string{scriptId}] =
      RunMetadata{.lastRunAt = QDateTime::currentSecsSinceEpoch(), .output = Utils::toBase64(line)};

  syncWithDisk();
}

std::optional<std::string> ScriptMetadataStore::lastRunData(const std::string &id) const {
  if (const auto it = m_data.inlineRuns.find(id); it != m_data.inlineRuns.end()) {
    return Utils::fromBase64(it->second.output);
  }

  return {};
}

std::optional<std::string> ScriptMetadataStore::syncWithDisk() {
  std::string buf;

  if (const auto error = glz::write_file_json(m_data, m_path.c_str(), buf)) {
    return glz::format_error(error);
  }

  return {};
}

std::expected<ScriptMetadataStore::Data, std::string> ScriptMetadataStore::loadDataFromDisk() {
  Data data;
  std::string buf;
  if (const auto error = glz::read_file_json(data, m_path.c_str(), buf)) {
    return std::unexpected(glz::format_error(error));
  }
  return data;
}

template <> struct glz::meta<ScriptMetadataStore::Data> : glz::snake_case {};
template <> struct glz::meta<ScriptMetadataStore::RunMetadata> : glz::snake_case {};
