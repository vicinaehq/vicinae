#include <cstdint>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <unistd.h>
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/indexer-service.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/migrations.hpp"
#include "file-indexer/util.hpp"

namespace {

bool shouldPurgeDatabase() {
  std::error_code ec;
  if (!std::filesystem::exists(file_indexer::databasePath(), ec)) return false;

  FileIndexerDatabase startupDb;
  if (!startupDb.isOpen()) {
    flog::warn() << "Existing file indexer database could not be opened, starting over";
    return true;
  }

  int const userVersion = startupDb.userVersion();
  if (userVersion == file_indexer::SCHEMA_VERSION) return false;

  flog::info() << "Breaking change detected (current rev=" << file_indexer::SCHEMA_VERSION
               << ", user version=" << userVersion << "), starting over... \n";
  return true;
}

} // namespace

class StdoutTransport : public file_indexer_gen::AbstractTransport {
  std::mutex m_mtx;

  void send(std::string_view data) override {
    std::scoped_lock const lock(m_mtx);
    uint32_t size = data.size();
    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(data.data(), data.size());
    std::cout.flush();
  }
};

int main(int, char **) {
  file_indexer::removeLegacyDbFiles();

  if (shouldPurgeDatabase()) { file_indexer::purgeDbFiles(); }

  StdoutTransport transport;
  file_indexer_gen::RpcTransport rpc{transport};
  file_indexer::IndexerService service{rpc};
  file_indexer_gen::Server server{rpc, service};

  service.listen(server);
}
