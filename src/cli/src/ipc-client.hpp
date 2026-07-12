#pragma once
#include <common/common.hpp>
#include <cctype>
#include <common/enumerate.hpp>
#include <expected>
#include <format>
#include <optional>
#include <string>
#include "local-socket.hpp"
#include "generated/ipc-client.hpp"

namespace cli {

inline std::string percentEncode(std::string_view value) {
  std::string encoded;
  encoded.reserve(value.size() * 3);

  for (unsigned char ch : value) {
    if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
      encoded.push_back(static_cast<char>(ch));
      continue;
    }

    encoded.append(std::format("%{:02X}", static_cast<int>(ch)));
  }

  return encoded;
}

class IpcClient {
public:
  static std::expected<IpcClient, std::string> connect() {
    return LocalSocket::connect(vicinae::serverSocketName()).transform([](LocalSocket sock) {
      return IpcClient(std::move(sock));
    });
  }

  ~IpcClient() = default;

  IpcClient(IpcClient &&other) noexcept
      : m_sock(std::move(other.m_sock)), m_transport(m_sock), m_rpc(m_transport), m_client(m_rpc) {}

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
    bool const hasArguments = fullUrl.contains('?');
    for (const auto &[idx, arg] : query | vicinae::enumerate) {
      char const separator = idx == 0 ? (hasArguments ? '&' : '?') : '&';
      fullUrl.append(std::format("{}{}={}", separator, percentEncode(arg.first), percentEncode(arg.second)));
    }
    return connect().and_then([&](IpcClient client) { return client.deeplink({.url = std::move(fullUrl)}); });
  }

  std::expected<ipc::DescribeResponse, std::string> describe() {
    return call<ipc::DescribeResponse>([&](auto cb) { m_client.ipc().describe(std::move(cb)); });
  }

  std::expected<ipc::LaunchAppResponse, std::string> launchApp(ipc::LaunchAppRequest req) {
    return call<ipc::LaunchAppResponse>([&](auto cb) { m_client.ipc().launchApp(req, std::move(cb)); });
  }

  std::expected<std::vector<ipc::FileResult>, std::string> fsQuery(std::string_view query, int limit = 100,
                                                                   std::optional<std::string> category = {}) {
    ipc::FsQueryParams params{
        .limit = limit,
        .category = std::move(category),
    };

    return call<std::vector<ipc::FileResult>>(
        [&](auto cb) { m_client.ipc().fsQuery(std::string{query}, params, std::move(cb)); });
  }

  std::expected<ipc::DMenuResponse, std::string> dmenu(ipc::DMenuRequest req) {
    return call<ipc::DMenuResponse>([&](auto cb) { m_client.ipc().dmenu(req, std::move(cb)); });
  }

private:
  IpcClient(LocalSocket sock)
      : m_sock(std::move(sock)), m_transport(m_sock), m_rpc(m_transport), m_client(m_rpc) {}

  struct SocketTransport : public ipc::AbstractTransport {
    LocalSocket &sock;
    explicit SocketTransport(LocalSocket &sock) : sock(sock) {}
    void send(std::string_view data) override {
      uint32_t size = data.size();
      sock.writeAll(&size, sizeof(size));
      sock.writeAll(data.data(), data.size());
    }
  };

  std::expected<std::string, std::string> recv() {
    uint32_t size;
    if (!m_sock.readAll(&size, sizeof(size))) return std::unexpected("Failed to read response size");

    std::string buf(size, '\0');
    if (!m_sock.readAll(buf.data(), size)) return std::unexpected("Failed to read response data");
    return buf;
  }

  template <typename T>
  std::expected<T, std::string>
  call(std::function<void(std::function<void(std::expected<T, std::string>)>)> fn) {
    std::expected<T, std::string> result = std::unexpected(std::string{"no response"});
    fn([&result](std::expected<T, std::string> res) { result = std::move(res); });

    auto data = recv();
    if (!data) return std::unexpected(data.error());
    if (auto res = m_client.route(*data); !res) return std::unexpected(res.error());

    return result;
  }

  LocalSocket m_sock;
  SocketTransport m_transport;
  ipc::RpcTransport m_rpc;
  ipc::Client m_client;
};

} // namespace cli
