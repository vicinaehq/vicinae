#include <cstring>
#include <filesystem>
#include <expected>
#include <format>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/util/string_literal.hpp>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <glaze/json.hpp>
#include "ipc.hpp"
#include <unistd.h>

namespace ipc {

using CliSchema = RpcSchema<ipc::Ping, ipc::DMenu, ipc::Deeplink, ipc::ListApps, ipc::LaunchApp>;
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

  void shutdown() {}

  void sendRaw(std::string_view data) {
    uint32_t size = data.size();
    ::send(m_sock, reinterpret_cast<const char *>(&size), sizeof(size), 0);
    ::send(m_sock, data.data(), data.size(), 0);
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
    int n;

    sendRaw(m_rpc.template request<T>(payload));
    n = ::recv(m_sock, reinterpret_cast<char *>(&size), sizeof(size), 0);
    data.resize(size);
    n = ::recv(m_sock, data.data(), data.size(), 0);

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
  static auto deeplink(std::string_view url) { return oneshot<ipc::Deeplink>({.url = std::string(url)}); }
};

/**
 * Client to communicate from a browser extension native host to the vicinae daemon
 */
class BrowserExtensionClient : public Client<BrowserExtensionSchema> {};

} // namespace ipc
