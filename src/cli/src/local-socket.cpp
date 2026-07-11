#include "local-socket.hpp"
#include <format>

#ifdef _WIN32
#include <windows.h>
#else
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif

namespace cli {

#ifdef _WIN32

std::expected<LocalSocket, std::string> LocalSocket::connect(const std::string &name) {
  std::string pipe = "\\\\.\\pipe\\" + name;
  HANDLE h = CreateFileA(pipe.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

  if (h == INVALID_HANDLE_VALUE)
    return std::unexpected(std::format("Failed to connect to {}: error {}", pipe, GetLastError()));

  return LocalSocket(h);
}

LocalSocket::~LocalSocket() {
  if (m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr) CloseHandle(m_handle);
}

LocalSocket::LocalSocket(LocalSocket &&other) noexcept : m_handle(other.m_handle) {
  other.m_handle = INVALID_HANDLE_VALUE;
}

LocalSocket &LocalSocket::operator=(LocalSocket &&other) noexcept {
  if (this != &other) {
    if (m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr) CloseHandle(m_handle);
    m_handle = other.m_handle;
    other.m_handle = INVALID_HANDLE_VALUE;
  }
  return *this;
}

bool LocalSocket::writeAll(const void *data, size_t n) {
  const char *p = static_cast<const char *>(data);
  size_t total = 0;
  while (total < n) {
    DWORD written = 0;
    if (!WriteFile(m_handle, p + total, static_cast<DWORD>(n - total), &written, nullptr) || written == 0)
      return false;
    total += written;
  }
  return true;
}

bool LocalSocket::readAll(void *data, size_t n) {
  char *p = static_cast<char *>(data);
  size_t total = 0;
  while (total < n) {
    DWORD read = 0;
    if (!ReadFile(m_handle, p + total, static_cast<DWORD>(n - total), &read, nullptr) || read == 0)
      return false;
    total += read;
  }
  return true;
}

#else

std::expected<LocalSocket, std::string> LocalSocket::connect(const std::string &name) {
  int const fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) return std::unexpected(std::format("Failed to create socket: {}", strerror(errno)));

  struct sockaddr_un addr{.sun_family = AF_UNIX};
  strncpy(addr.sun_path, name.data(), sizeof(addr.sun_path) - 1);

  if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
    ::close(fd);
    return std::unexpected(std::format("Failed to connect to {}: {}", name, strerror(errno)));
  }

  return LocalSocket(fd);
}

LocalSocket::~LocalSocket() {
  if (m_fd >= 0) ::close(m_fd);
}

LocalSocket::LocalSocket(LocalSocket &&other) noexcept : m_fd(other.m_fd) { other.m_fd = -1; }

LocalSocket &LocalSocket::operator=(LocalSocket &&other) noexcept {
  if (this != &other) {
    if (m_fd >= 0) ::close(m_fd);
    m_fd = other.m_fd;
    other.m_fd = -1;
  }
  return *this;
}

bool LocalSocket::writeAll(const void *data, size_t n) {
  const char *p = static_cast<const char *>(data);
  size_t total = 0;
  while (total < n) {
    auto sent = ::send(m_fd, p + total, n - total, 0);
    if (sent <= 0) return false;
    total += static_cast<size_t>(sent);
  }
  return true;
}

bool LocalSocket::readAll(void *data, size_t n) {
  char *p = static_cast<char *>(data);
  size_t total = 0;
  while (total < n) {
    auto got = ::recv(m_fd, p + total, n - total, 0);
    if (got <= 0) return false;
    total += static_cast<size_t>(got);
  }
  return true;
}

#endif

} // namespace cli
