#pragma once
#include <cstddef>
#include <expected>
#include <string>

namespace cli {

class LocalSocket {
public:
  static std::expected<LocalSocket, std::string> connect(const std::string &name);

  ~LocalSocket();
  LocalSocket(LocalSocket &&other) noexcept;
  LocalSocket &operator=(LocalSocket &&other) noexcept;
  LocalSocket(const LocalSocket &) = delete;
  LocalSocket &operator=(const LocalSocket &) = delete;

  bool writeAll(const void *data, size_t n);
  bool readAll(void *data, size_t n);

private:
#ifdef _WIN32
  explicit LocalSocket(void *handle) : m_handle(handle) {}
  void *m_handle;
#else
  explicit LocalSocket(int fd) : m_fd(fd) {}
  int m_fd;
#endif
};

} // namespace cli
