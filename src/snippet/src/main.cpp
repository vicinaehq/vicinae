#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include "snippet/snippet.hpp"

class StdoutTransport : public snippet_gen::AbstractTransport {
  void send(std::string_view data) override {
    uint32_t size = data.size();
    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(data.data(), data.size());
    std::cout.flush();
  }
};

int main(int, char **) {
  StdoutTransport transport;
  snippet_gen::RpcTransport rpc{transport};
  snippet::SnippetService service{rpc};
  snippet_gen::Server server{rpc, service};

  service.listen(server);
}
