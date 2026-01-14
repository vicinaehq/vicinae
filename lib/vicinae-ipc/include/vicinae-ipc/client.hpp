#include <cstring>
#include <filesystem>
#include <expected>
#include <format>
#include <glaze/core/reflect.hpp>
#include <glaze/util/string_literal.hpp>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <glaze/json.hpp>
#include "ipc.hpp"
#include <unistd.h>

namespace ipc {
class Client {
  using Schema =
      RpcSchema<ipc::Handshake, ipc::Ping, ipc::DMenu, ipc::Deeplink, ipc::ListApps, ipc::LaunchApp>;

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

  void dmenu(auto cb) {
    // std::string uuid{"42"};
    // auto [request, inserted] = m_client.request<"dmenu">(uuid, DMenu::Request(), cb);
    // sendRaw(request);
  }

  void shutdown() {}

  void sendRaw(std::string_view data) {
    uint32_t size = data.size();
    ::send(m_sock, reinterpret_cast<const char *>(&size), sizeof(size), 0);
    ::send(m_sock, data.data(), data.size(), 0);
  }

  template <ipc::IsCommandVerb T> static auto oneshot(T::Request payload) {
    return ipc::Client::make().and_then(
        [payload = std::move(payload)](Client client) -> std::expected<typename T::Response, std::string> {
          if (const auto result = client.connect(); !result) { return std::unexpected(result.error()); }

          return client.request<ipc::Handshake>({.clientType = ClientType::CommandDispatcher})
              .and_then([&](const auto &res) { return client.request<T>(payload); });
        });
  }

  static auto deeplink(std::string_view url) { return oneshot<ipc::Deeplink>({.url = std::string(url)}); }

  /**
   */
  template <InSchema<Schema> T> std::expected<typename T::Response, std::string> request(T::Request payload) {
    int id = 0;

    std::string data;
    uint32_t size;
    int n;

    std::string json = m_rpc.request<T>(payload);

    sendRaw(json);

    n = ::recv(m_sock, reinterpret_cast<char *>(&size), sizeof(size), 0);
    data.resize(size);
    n = ::recv(m_sock, data.data(), data.size(), 0);

    Schema::Response res;

    if (const auto error = glz::read_json(res, data)) {
      return std::unexpected(std::format("Failed to parse response: {}", glz::format_error(error)));
    }

    if (res.error) { return std::unexpected(res.error->message); }

    if (auto matchingRes = std::get_if<typename T::Response>(&res.result.value())) { return *matchingRes; }

    return std::unexpected("Returned response if not of expected type for request");
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

  // ipc::RpcClient m_client;
  // ipc::RpcServer m_receiver;
};

} // namespace ipc
