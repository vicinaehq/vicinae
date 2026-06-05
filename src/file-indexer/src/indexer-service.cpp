#include "file-indexer/indexer-service.hpp"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <glaze/glaze.hpp>
#include <thread>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

namespace file_indexer {

// Named namespace (not anonymous): glaze's reflection needs the type to have linkage,
// otherwise clang rejects it with "used but not defined ... type does not have linkage".
struct MethodPeek {
  std::string method;
};

static bool isQueryMessage(std::string_view payload) {
  MethodPeek peek;
  if (glz::read<glz::opts{.error_on_unknown_keys = false}>(peek, payload)) { return false; }
  return peek.method == "FileIndexer/query";
}

IndexerService::IndexerService(file_indexer_gen::RpcTransport &transport)
    : file_indexer_gen::AbstractFileIndexer(transport) {}

static std::vector<fs::path> toPaths(const std::vector<std::string> &values) {
  std::vector<fs::path> paths;
  paths.reserve(values.size());
  for (const auto &value : values) {
    paths.emplace_back(value);
  }
  return paths;
}

std::expected<void, std::string> IndexerService::configure(file_indexer_gen::IndexerConfig config) {
  m_indexer.setConfig(toPaths(config.paths), toPaths(config.excluded_paths), toPaths(config.watcher_paths));

  if (!m_started) {
    m_started = true;
    m_indexer.start();
  }

  return {};
}

std::expected<void, std::string> IndexerService::rebuildIndex() {
  m_indexer.rebuildIndex();
  return {};
}

std::expected<file_indexer_gen::QueryResponse, std::string>
IndexerService::query(file_indexer_gen::QueryRequest req) {
  Pagination const pagination{.offset = req.pagination.offset, .limit = req.pagination.limit};
  auto results = m_indexer.query(req.text, pagination);

  file_indexer_gen::QueryResponse response;
  response.matches.reserve(results.size());
  for (const auto &result : results) {
    response.matches.emplace_back(
        file_indexer_gen::FileMatch{.path = result.path.string(), .rank = result.rank});
  }

  return response;
}

void IndexerService::listen(file_indexer_gen::Server &server) {
  std::vector<char> buffer;
  char tmp[4096];

  while (true) {
    const ssize_t n = read(STDIN_FILENO, tmp, sizeof(tmp));
    if (n <= 0) break;

    buffer.insert(buffer.end(), tmp, tmp + n);

    constexpr size_t HEADER_SIZE = sizeof(uint32_t);

    while (buffer.size() >= HEADER_SIZE) {
      uint32_t frameLen = 0;
      std::memcpy(&frameLen, buffer.data(), HEADER_SIZE);

      if (buffer.size() - HEADER_SIZE < frameLen) break;

      std::string_view payload{buffer.data() + HEADER_SIZE, frameLen};

      // Queries run off-thread so a slow one can't block keystrokes behind it; control
      // messages stay inline. Handlers are reentrant: each query opens its own connection.
      if (isQueryMessage(payload)) {
        std::thread([&server, msg = std::string(payload)]() { server.route(msg); }).detach();
      } else {
        server.route(payload);
      }

      buffer.erase(buffer.begin(), buffer.begin() + HEADER_SIZE + frameLen);
    }
  }
}

}; // namespace file_indexer
