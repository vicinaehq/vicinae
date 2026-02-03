#include <cstring>
#include <filesystem>
#include <expected>
#include <format>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/util/string_literal.hpp>
#include <netinet/in.h>
#include <ranges>
#include <sys/socket.h>
#include <sys/un.h>
#include "ipc.hpp"
#include <unistd.h>

namespace ipc {

using CliSchema = RpcSchema<ipc::Ping, ipc::DMenu, ipc::Deeplink, ipc::LaunchApp>;
using BrowserExtensionSchema = RpcSchema<ipc::Ping, ipc::BrowserInit, ipc::BrowserTabsChanged>;

template <IsRpcSchema SchemaType = CliSchema> class Client {
  using Schema = SchemaType;

public:
  static std::filesystem::path vicinaeSocketPath() {
    return std::filesystem::path(getenv("XDG_RUNTIME_DIR")) / "vicinae" / "vicinae.sock";
  }

  static std::expected<Client, std::string> make() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) { return std::unexpected(std::format("Failed to create socket: {}", strerror(errno))); }

    return Client(fd);
  }

  ~Client() { close(m_sock); }

  std::expected<void, std::string> connect() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) { return std::unexpected(std::format("Failed to create socket: {}", strerror(errno))); }

    struct sockaddr_un addr{.sun_family = AF_UNIX};

    strncpy(addr.sun_path, m_path.data(), m_path.size());

    if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
      return std::unexpected(std::format("Failed to connect to socket at {}: {}", m_path, strerror(errno)));
    }

    m_connected = true;

    return {};
  }

  bool sendRaw(std::string_view data) {
    uint32_t size = data.size();
    if (::send(m_sock, reinterpret_cast<const char *>(&size), sizeof(size), 0) < sizeof(size)) {
      return false;
    }
    if (::send(m_sock, data.data(), data.size(), 0) < data.size()) { return false; }
    return true;
  }

  template <InSchema<Schema> T> static auto oneshot(T::Request payload) {
    return ipc::Client<SchemaType>::make().and_then(
        [payload = std::move(payload)](Client client) -> std::expected<typename T::Response, std::string> {
          if (const auto result = client.connect(); !result) { return std::unexpected(result.error()); }
          return client.template request<T>(payload);
        });
  }

  template <InSchema<Schema> T> std::expected<void, std::string> notify(const T::Request &payload) {
    const std::string json = m_rpc.template notify<T>(payload);
    sendRaw(json);
    return {};
  }

  /**
   * Make a request and block until we get a response.
   */
  template <InSchema<Schema> T> std::expected<typename T::Response, std::string> request(T::Request payload) {
    int id = 0;
    std::string data;
    uint32_t size;

    sendRaw(m_rpc.template request<T>(payload));

    {
      if (::recv(m_sock, reinterpret_cast<char *>(&size), sizeof(size), 0) < sizeof(size)) {
        return std::unexpected("Failed to read response size");
      }

      data.resize(size);

      if (::recv(m_sock, data.data(), data.size(), 0) < data.size()) {
        return std::unexpected("Failed to read response data");
      }

      typename Schema::Response res;

      if (const auto error = glz::read_json(res, data)) {
        return std::unexpected(std::format("Failed to parse response: {}", glz::format_error(error)));
      }

      if (res.error) { return std::unexpected(res.error->message); }

      typename T::Response resData;

      if (const auto error = glz::read_json(resData, res.result->str)) {
        return std::unexpected(std::format("Failed to read response data: {}", glz::format_error(error)));
      }

      return resData;
    }
  }

  /**
   * Socket file descriptor
   */
  int handle() const { return m_sock; }

private:
  Client(int fd, const std::string &path = vicinaeSocketPath()) : m_sock(fd), m_path(path) {}

  int m_sock = -1;
  bool m_connected = false;
  std::string m_path;
  RpcClient<Schema> m_rpc;
};

struct CliClient : public Client<CliSchema> {
  struct DeeplinkOptions {
    std::vector<std::pair<std::string, std::string>> query;
  };

  static auto deeplink(std::string_view url, const DeeplinkOptions &opts = {}) {
    std::string fullUrl{url};

    if (!opts.query.empty()) {
      for (const auto &[idx, arg] : opts.query | std::views::enumerate) {
        fullUrl.append(std::format("{}{}={}", idx == 0 ? "?" : "&", arg.first, arg.second));
      }
    }

    return oneshot<ipc::Deeplink>({.url = std::string(fullUrl)});
  }
};

/**
 * Client to communicate from a browser extension native host to the vicinae daemon
 */
class BrowserExtensionClient : public Client<BrowserExtensionSchema> {};

} // namespace ipc
