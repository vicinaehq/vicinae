#include "file-indexer/indexer-service.hpp"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <functional>
#include <thread>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

namespace file_indexer {

static file_indexer_gen::ScanKind toScanKind(ScanType type) {
  switch (type) {
  case ScanType::Full:
    return file_indexer_gen::ScanKind::Full;
  case ScanType::Incremental:
    return file_indexer_gen::ScanKind::Incremental;
  case ScanType::Watcher:
    return file_indexer_gen::ScanKind::Watcher;
  }
  return file_indexer_gen::ScanKind::Full;
}

static file_indexer_gen::ScanState toScanState(ScanStatus status) {
  switch (status) {
  case ScanStatus::Succeeded:
    return file_indexer_gen::ScanState::Succeeded;
  case ScanStatus::Failed:
    return file_indexer_gen::ScanState::Failed;
  case ScanStatus::Interrupted:
    return file_indexer_gen::ScanState::Interrupted;
  case ScanStatus::Pending:
  case ScanStatus::Started:
    return file_indexer_gen::ScanState::Started;
  }
  return file_indexer_gen::ScanState::Started;
}

IndexerService::IndexerService(file_indexer_gen::RpcTransport &transport)
    : file_indexer_gen::AbstractFileIndexer(transport) {
  m_indexer.setScanEventCallback([this](const ScanEvent &event) {
    emitscanStatusChanged({.scan_id = event.scanId,
                           .kind = toScanKind(event.type),
                           .state = toScanState(event.status),
                           .entrypoint = event.entrypoint.string(),
                           .processed_file_count = static_cast<uint32_t>(event.processedFileCount)});
  });
}

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

void IndexerService::query(
    file_indexer_gen::QueryRequest req,
    std::function<void(std::expected<file_indexer_gen::QueryResponse, std::string>)> reply) {
  // Run off-thread so a slow query can't block the read loop / other queries.
  std::thread([this, req = std::move(req), reply = std::move(reply)]() {
    auto results = m_indexer.query(req.text, req.limit);

    file_indexer_gen::QueryResponse response;
    response.matches.reserve(results.size());
    for (const auto &result : results) {
      response.matches.emplace_back(
          file_indexer_gen::FileMatch{.path = result.path.string(), .rank = result.rank});
    }

    reply(response);
  }).detach();
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
      server.route(payload);

      buffer.erase(buffer.begin(), buffer.begin() + HEADER_SIZE + frameLen);
    }
  }
}

}; // namespace file_indexer
