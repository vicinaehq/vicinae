#include <cstdint>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/generic.hpp>
#include <glaze/json.hpp>
#include <istream>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <iostream>
#include <glaze/glaze.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "proto/daemon.pb.h"

namespace wire = proto::ext::daemon;

class VicinaeClient {
public:
  static std::filesystem::path vicinaeSocketPath() {
    return std::filesystem::path(getenv("XDG_RUNTIME_DIR")) / "vicinae" / "vicinae.sock";
  }

  static std::expected<VicinaeClient, std::string> make() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) { return std::unexpected(std::format("Failed to create socket: {}", strerror(errno))); }

    return VicinaeClient(fd);
  }

  ~VicinaeClient() { close(m_sock); }

  wire::HandshakeResponse handshake(wire::ClientType type) {
    wire::Request req;
    auto handshake = new wire::HandshakeRequest();
    handshake->set_type(type);
    req.set_allocated_handshake(handshake);
    auto res = sendRequestSync(req);
    return res.handshake();
  }

  bool connect() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
      std::println(std::cerr, "Failed to create socket: {}", strerror(errno));
      return false;
    }

    struct sockaddr_un addr{.sun_family = AF_UNIX};

    strncpy(addr.sun_path, m_path.data(), m_path.size());

    if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
      std::println(std::cerr, "Failed to connect to socket at {}: {}", m_path, strerror(errno));
    }

    return true;
  }

  wire::Response sendRequestSync(wire::Request req) {
    {
      std::string payload = req.SerializeAsString();
      uint32_t size = htonl(payload.size());

      ::send(m_sock, reinterpret_cast<char *>(&size), sizeof(size), 0);
      ::send(m_sock, payload.data(), payload.size(), 0);
    }

    uint32_t size = 0;
    std::string responseData;

    std::println(std::cerr, "waiting for response length {}", size);
    recv(m_sock, reinterpret_cast<char *>(&size), sizeof(size), 0);
    size = ntohl(size);
    std::println(std::cerr, "got response of size {}", size);
    responseData.resize(size);
    recv(m_sock, responseData.data(), responseData.size(), 0);

    wire::Response res;

    res.ParseFromString(responseData);

    return res;
  }

  void sendRaw(std::string_view data) { ::send(m_sock, data.data(), data.size(), 0); }

  /**
   * Socket file descriptor
   */
  int handle() const { return m_sock; }

private:
  VicinaeClient(int fd, const std::string &path = vicinaeSocketPath()) : m_sock(fd), m_path(path) {}

  int m_sock = -1;
  std::string m_path;
};

struct BrowserTab {
  int id;
  std::string title;
  std::string url;
  int windowId;
  bool active;

  operator wire::BrowserTabInfo() const {
    wire::BrowserTabInfo info;
    info.set_id(id);
    info.set_title(title);
    info.set_url(url);
    info.set_window_id(windowId);
    info.set_active(active);
    info.set_active(active);
    return info;
  }
};

using TabList = std::vector<BrowserTab>;

struct VersionRequest {};

using IncomingMessageData = std::variant<TabList, VersionRequest>;

template <> struct glz::meta<IncomingMessageData> {
  static constexpr std::string_view tag = "type";
  static constexpr auto ids = std::array{"tab_list", "version"};
};

struct IncomingExtensionMessage {
  std::string type;
  IncomingMessageData data;
};

struct VersionInfo {
  std::string tag;
  std::int64_t pid;
};

struct OutgoingExtensionMessage {
  std::string type;
  std::variant<VersionInfo> data;
};

static bool readMessage(std::istream &ifs, std::string &data) {
  uint32_t length = 0;

  ifs.read(reinterpret_cast<char *>(&length), sizeof(length));
  data.resize(length);
  ifs.read(data.data(), data.size());

  return !(ifs.eof() && data.size() == 0);
}

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename T, class... Ts> static auto match(const T &type, overloads<Ts...> visitor) {
  return std::visit(visitor, type);
}

static bool sendMessage(OutgoingExtensionMessage message) {

  std::string buf;

  if (const auto error = glz::write_json(message, buf)) {
    std::println(std::cerr, "Failed to serialize json: {}", glz::format_error(error));
    return false;
  }

  std::println(std::cerr, "wrote {}", buf);

  uint32_t size = buf.size();

  std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
  std::cout.write(buf.data(), buf.size());
  std::cout.flush();

  return true;
}

int main() {
  if (isatty(STDIN_FILENO)) {
    std::println(
        std::cerr,
        "This executable is meant to be started by the vicinae browser extension using native host messaging "
        "(https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging). Do not "
        "start manually.");
    return 1;
  }

  std::string buf;

  auto client = VicinaeClient::make().value();

  if (!client.connect()) {
    std::cerr << "Failed to connect to vicinae";
    return 1;
  }

  auto handshake = client.handshake(wire::ClientType::BrowserExtension);

  std::println(std::cerr, "Got handshake response from vicinae {}", handshake.version());

  if (!handshake.ok()) {
    std::println(std::cerr, "Handshake failed");
    return 1;
  }

  std::println(std::cerr, "Listening for messages...");

  while (readMessage(std::cin, buf)) {
    IncomingExtensionMessage msg;

    if (const auto error = glz::read_json(msg, buf)) {
      std::println(std::cerr, "\"{}\"", buf);
      std::println(std::cerr, "Failed to parse json: {}", glz::format_error(error));
      continue;
    }

    std::println(std::cerr, "Processing message: {}", buf);

    match(msg.data, overloads{[&](const TabList &tabs) {
                                wire::Request req;
                                auto tabsChanged = std::make_unique<wire::BrowserTabChangedRequest>();

                                for (const auto &tab : tabs) {
                                  auto data = tabsChanged->add_tabs();
                                  *data = tab;
                                }

                                req.set_allocated_browser_tabs_changed(tabsChanged.release());
                                client.sendRequestSync(req);
                              },
                              [&](const VersionRequest &req) {
                                VersionInfo info;
                                sendMessage(OutgoingExtensionMessage(
                                    msg.type, VersionInfo(handshake.version(), handshake.pid())));
                              }});
  }

  std::println(std::cerr, "Got EOF, exiting...");
}
