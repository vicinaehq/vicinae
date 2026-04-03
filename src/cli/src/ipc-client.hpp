#pragma once
#include <cstring>
#include <expected>
#include <filesystem>
#include <format>
#include <ranges>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "generated/ipc-client.hpp"

namespace cli {

inline std::filesystem::path socketPath() {
  return std::filesystem::path(getenv("XDG_RUNTIME_DIR")) / "vicinae" / "vicinae.sock";
}

class IpcClient {
public:
  static std::expected<IpcClient, std::string> connect() {
    int const fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) return std::unexpected(std::format("Failed to create socket: {}", strerror(errno)));

    auto path = socketPath().string();
    struct sockaddr_un addr{.sun_family = AF_UNIX};
    strncpy(addr.sun_path, path.data(), path.size());

    if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
      ::close(fd);
      return std::unexpected(std::format("Failed to connect to {}: {}", path, strerror(errno)));
    }

    return IpcClient(fd);
  }

  ~IpcClient() {
    if (m_fd >= 0) ::close(m_fd);
  }

  IpcClient(IpcClient &&other) noexcept
      : m_fd(other.m_fd), m_transport(m_fd), m_rpc(m_transport), m_client(m_rpc) {
    other.m_fd = -1;
  }

  IpcClient(const IpcClient &) = delete;
  IpcClient &operator=(const IpcClient &) = delete;
  IpcClient &operator=(IpcClient &&) = delete;

  std::expected<ipc::PingResponse, std::string> ping() {
    return call<ipc::PingResponse>([&](auto cb) { m_client.ipc().ping(std::move(cb)); });
  }

  std::expected<ipc::DeeplinkResponse, std::string> deeplink(ipc::DeeplinkRequest req) {
    return call<ipc::DeeplinkResponse>([&](auto cb) { m_client.ipc().deeplink(req, std::move(cb)); });
  }

  static std::expected<ipc::DeeplinkResponse, std::string>
  sendDeeplink(std::string_view url, const std::vector<std::pair<std::string, std::string>> &query = {}) {
    std::string fullUrl{url};
    for (const auto &[idx, arg] : query | std::views::enumerate) {
      fullUrl.append(std::format("{}{}={}", idx == 0 ? "?" : "&", arg.first, arg.second));
    }
    return connect().and_then([&](IpcClient client) { return client.deeplink({.url = std::move(fullUrl)}); });
  }

  std::expected<ipc::DescribeResponse, std::string> describe() {
    return call<ipc::DescribeResponse>([&](auto cb) { m_client.ipc().describe(std::move(cb)); });
  }

  std::expected<ipc::LaunchAppResponse, std::string> launchApp(ipc::LaunchAppRequest req) {
    return call<ipc::LaunchAppResponse>([&](auto cb) { m_client.ipc().launchApp(req, std::move(cb)); });
  }

  std::expected<ipc::DMenuResponse, std::string> dmenu(ipc::DMenuRequest req) {
    return call<ipc::DMenuResponse>([&](auto cb) { m_client.ipc().dmenu(req, std::move(cb)); });
  }

private:
  IpcClient(int fd) : m_fd(fd), m_transport(m_fd), m_rpc(m_transport), m_client(m_rpc) {}

  struct SocketTransport : public ipc::AbstractTransport {
    int fd;
    explicit SocketTransport(int &fd) : fd(fd) {}
    void send(std::string_view data) override {
      uint32_t size = data.size();
      ::send(fd, &size, sizeof(size), 0);
      ::send(fd, data.data(), data.size(), 0);
    }
  };

  std::expected<std::string, std::string> recv() {
    uint32_t size;
    if (::recv(m_fd, &size, sizeof(size), 0) < static_cast<ssize_t>(sizeof(size)))
      return std::unexpected("Failed to read response size");

    std::string buf(size, '\0');
    size_t total = 0;
    while (total < size) {
      auto n = ::recv(m_fd, buf.data() + total, size - total, 0);
      if (n <= 0) return std::unexpected("Failed to read response data");
      total += n;
    }
    return buf;
  }

  template <typename T>
  std::expected<T, std::string>
  call(std::function<void(std::function<void(std::expected<T, std::string>)>)> fn) {
    std::expected<T, std::string> result = std::unexpected(std::string{"no response"});
    fn([&result](std::expected<T, std::string> res) { result = std::move(res); });

    auto data = recv();
    if (!data) return std::unexpected(data.error());
    m_client.route(*data);

    return result;
  }

  int m_fd;
  SocketTransport m_transport;
  ipc::RpcTransport m_rpc;
  ipc::Client m_client;
};

} // namespace cli
