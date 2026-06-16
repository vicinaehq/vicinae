#pragma once
#include "file-indexer-server.hpp"
#include "file-indexer/file-indexer.hpp"
#include "file-indexer/query-pool.hpp"
#include <functional>

namespace file_indexer {

class IndexerService : public file_indexer_gen::AbstractFileIndexer {
public:
  IndexerService(file_indexer_gen::RpcTransport &transport);

  std::expected<void, std::string> configure(file_indexer_gen::IndexerConfig config) override;
  std::expected<void, std::string> rebuildIndex() override;
  void query(file_indexer_gen::QueryRequest req,
             std::function<void(std::expected<file_indexer_gen::QueryResponse, std::string>)> reply) override;

  void listen(file_indexer_gen::Server &server);

private:
  FileIndexer m_indexer;
  QueryPool m_queryPool;
  bool m_started = false;
};

}; // namespace file_indexer
