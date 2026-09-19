#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string_view>
#include <vector>
#include "inference-service.hpp"
#include "whisper.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace {

class StdoutTransport : public inference_gen::AbstractTransport {
  std::mutex m_mtx;

  void send(std::string_view data) override {
    std::scoped_lock const lock(m_mtx);
    std::uint32_t size = data.size();
    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(data.data(), data.size());
    std::cout.flush();
  }
};

// fread would block until the whole buffer is full, holding replies back by one request.
std::size_t readSome(char *buf, std::size_t size) {
#ifdef _WIN32
  const int n = _read(_fileno(stdin), buf, static_cast<unsigned>(size));
#else
  const ssize_t n = read(STDIN_FILENO, buf, size);
#endif
  return n > 0 ? static_cast<std::size_t>(n) : 0;
}

void serve(inference_gen::Server &server) {
  constexpr std::size_t HEADER_SIZE = sizeof(std::uint32_t);
  std::vector<char> buffer;
  std::array<char, 4096> tmp{};

  while (true) {
    const std::size_t n = readSome(tmp.data(), tmp.size());
    if (n == 0) break;

    buffer.insert(buffer.end(), tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(n));

    while (buffer.size() >= HEADER_SIZE) {
      std::uint32_t frameLen = 0;
      std::memcpy(&frameLen, buffer.data(), HEADER_SIZE);

      if (buffer.size() - HEADER_SIZE < frameLen) break;

      server.route(std::string_view{buffer.data() + HEADER_SIZE, frameLen});
      buffer.erase(buffer.begin(), buffer.begin() + HEADER_SIZE + frameLen);
    }
  }
}

void quietLog(ggml_log_level level, const char *text, void *) {
  if (level == GGML_LOG_LEVEL_WARN || level == GGML_LOG_LEVEL_ERROR) std::cerr << text << std::flush;
}

} // namespace

int main(int, char **) {
  whisper_log_set(quietLog, nullptr);
#ifdef _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  StdoutTransport transport;
  inference_gen::RpcTransport rpc{transport};
  inference::InferenceService service{rpc};
  inference_gen::Server server{rpc, service};

  serve(server);
}
