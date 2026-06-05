#pragma once
#include "file-indexer-server.hpp"
#include "file-indexer/file-indexer.hpp"

namespace file_indexer {

class IndexerService : public file_indexer_gen::AbstractFileIndexer {
  FileIndexer m_indexer;
  bool m_started = false;

public:
  IndexerService(file_indexer_gen::RpcTransport &transport);

  std::expected<void, std::string> configure(file_indexer_gen::IndexerConfig config) override;
  std::expected<void, std::string> rebuildIndex() override;
  std::expected<file_indexer_gen::QueryResponse, std::string>
  query(file_indexer_gen::QueryRequest req) override;

  void listen(file_indexer_gen::Server &server);
};

}; // namespace file_indexer
