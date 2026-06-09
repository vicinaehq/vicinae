#include <cstdint>
#include <iostream>
#include <mutex>
#include <unistd.h>
#include "file-indexer/indexer-service.hpp"

// Locked so concurrent query replies don't interleave mid-frame.
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
  StdoutTransport transport;
  file_indexer_gen::RpcTransport rpc{transport};
  file_indexer::IndexerService service{rpc};
  file_indexer_gen::Server server{rpc, service};

  service.listen(server);
}
