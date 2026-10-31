#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include "file-tools.hpp"
#include "services/files-service/file-service.hpp"

namespace fs = std::filesystem;

template <> struct glz::meta<vicinae::FileCategory> {
  using enum vicinae::FileCategory;
  static constexpr auto value =
      glz::enumerate("other", Other, "directory", Directory, "image", Image, "video", Video, "audio", Audio,
                     "document", Document, "archive", Archive, "application", Application);
};

namespace {

constexpr int DEFAULT_LIMIT = 20;
constexpr int MAX_LIMIT = 50;
constexpr std::size_t MAX_READ_BYTES = 64 * 1024;

struct SearchInput {
  std::string query;
  std::optional<vicinae::FileCategory> category;
  std::optional<int> limit;
};

struct SearchResult {
  std::string path;
  std::optional<vicinae::FileCategory> category;
  std::optional<std::string> mime_type;
};

struct ReadInput {
  std::string path;
  std::optional<std::uint64_t> offset;
};

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

AI::AbstractTool::RawToolResult serialize(const std::vector<SearchResult> &results) {
  std::string out;
  if (const auto error = glz::write_json(results, out)) return std::unexpected(glz::format_error(error));
  return AI::ToolOutput{.content = std::move(out)};
}

fs::path expandHome(const std::string &path) {
  if (path == "~") return QDir::homePath().toStdString();
  if (path.starts_with("~/")) return fs::path(QDir::homePath().toStdString()) / path.substr(2);
  return path;
}

AI::AbstractTool::RawToolResult readChunk(const fs::path &path, std::uint64_t offset) {
  std::error_code ec;
  if (!path.is_absolute()) return std::unexpected(std::string("The path must be absolute."));
  if (fs::is_directory(path, ec)) return std::unexpected(std::format("{} is a directory.", path.string()));

  const auto size = fs::file_size(path, ec);
  if (ec) return std::unexpected(std::format("Could not read {}: {}", path.string(), ec.message()));
  if (offset > size)
    return std::unexpected(std::format("The offset is past the end of the file ({} bytes).", size));

  std::ifstream stream(path, std::ios::binary);
  if (!stream) return std::unexpected(std::format("Could not open {}.", path.string()));
  stream.seekg(static_cast<std::streamoff>(offset));

  std::string buffer(MAX_READ_BYTES + 1, '\0');
  stream.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  buffer.resize(static_cast<std::size_t>(stream.gcount()));

  std::string_view chunk = buffer;
  const auto skipped =
      std::ranges::find_if(chunk, [](char c) { return (static_cast<unsigned char>(c) & 0xC0) != 0x80; }) -
      chunk.begin();
  chunk.remove_prefix(static_cast<std::size_t>(skipped));

  const auto [text, truncated] = AI::truncateUtf8(chunk, MAX_READ_BYTES);
  if (!AI::isReadableText(text)) return std::unexpected(std::format("{} is not a text file.", path.string()));

  auto out = std::string(text);
  if (truncated) {
    const auto next = offset + skipped + text.size();
    out += std::format("\n\n[Showing bytes {} to {} of {}. Read again with offset {} to continue.]",
                       offset + skipped, next, size, next);
  }
  return AI::ToolOutput{.content = std::move(out)};
}

} // namespace

std::string SearchFilesTool::description() const {
  return "Search files on the user's computer by name using the file index. An empty query returns recently "
         "used files. Returns absolute paths.";
}

std::string SearchFilesTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"query":{"type":"string","description":"File name to search for. Empty to list recently used files."},"category":{"type":"string","enum":["directory","image","video","audio","document","archive","application","other"],"description":"Only return files of this category."},"limit":{"type":"integer","minimum":1,"maximum":50,"description":"Maximum number of results. Defaults to 20."}},"required":["query"],"additionalProperties":false})";
}

bool SearchFilesTool::isEnabled() const { return m_files.isAvailable(); }

std::optional<std::string> SearchFilesTool::invocationSummary(std::string_view arguments) const {
  SearchInput input;
  if (glz::read_json(input, arguments) || input.query.empty()) return std::nullopt;
  return input.query;
}

AI::AbstractTool::RawToolTask SearchFilesTool::runRaw(std::string_view arguments) {
  SearchInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  const auto limit = std::clamp(input.limit.value_or(DEFAULT_LIMIT), 1, MAX_LIMIT);

  if (input.query.empty()) {
    auto future = m_files.recentFilesAsync({.limit = limit, .category = input.category})
                      .then([](const std::vector<fs::path> &paths) {
                        std::vector<SearchResult> results;
                        results.reserve(paths.size());
                        for (const auto &path : paths)
                          results.emplace_back(SearchResult{.path = path.string()});
                        return serialize(results);
                      });
    return {std::move(future), {}};
  }

  auto future =
      m_files.queryAsync(input.query, {.limit = limit, .category = input.category})
          .then([](const std::vector<IndexerFileResult> &files) {
            std::vector<SearchResult> results;
            results.reserve(files.size());
            for (const auto &file : files)
              results.emplace_back(SearchResult{
                  .path = file.path.string(), .category = file.category, .mime_type = file.mimeType});
            return serialize(results);
          });
  return {std::move(future), {}};
}

std::string ReadFileTool::description() const {
  return "Read a text file on the user's computer. Returns up to 64 KiB starting at offset (in bytes); "
         "longer "
         "files say which offset to use to continue. Binary files cannot be read.";
}

std::string ReadFileTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"path":{"type":"string","description":"Absolute path of the file. ~ is expanded to the home directory."},"offset":{"type":"integer","minimum":0,"description":"Byte offset to start reading from. Defaults to 0."}},"required":["path"],"additionalProperties":false})";
}

std::optional<std::string> ReadFileTool::invocationSummary(std::string_view arguments) const {
  ReadInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.path;
}

AI::AbstractTool::RawToolTask ReadFileTool::runRaw(std::string_view arguments) {
  ReadInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  return {QtConcurrent::run([path = expandHome(input.path), offset = input.offset.value_or(0)] {
            return readChunk(path, offset);
          }),
          {}};
}
