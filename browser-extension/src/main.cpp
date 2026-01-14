#include <cassert>
#include <cstdint>
#include <cstring>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/generic.hpp>
#include <glaze/json.hpp>
#include "poll.h"
#include "vicinae-ipc/ipc.hpp"
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <iostream>
#include <glaze/glaze.hpp>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <zconf-ng.h>

struct BrowserTab {
  int id;
  std::string title;
  std::string url;
  int windowId;
  bool active;
};

using TabList = std::vector<BrowserTab>;

struct VersionRequest {};

struct VersionInfo {
  std::string tag;
  std::int64_t pid;
};

struct OutgoingExtensionMessage {
  std::string type;
  std::variant<VersionInfo> data;
};

class Frame {
public:
  static constexpr auto READ_SIZE = 1;

  enum class ByteOrder : std::uint8_t { Host, Network };

  using Handler = std::function<void(std::string_view message)>;

  Frame(ByteOrder bo, Handler fn) : m_bo(bo), m_fn(fn) {}

  uint32_t transformSize(uint32_t s) { return m_bo == ByteOrder::Host ? s : ntohl(s); }

  void readPart(int fd) {
    for (;;) {
      int rc = read(fd, m_buf.data(), m_buf.size());

      if (rc == -1) {
        std::println(std::cerr, "Failed to read fd {}: {}", fd, strerror(errno));
        return;
      }

      data.append(std::string_view(m_buf.data(), rc));

      for (;;) {
        if (size == 0 && data.size() >= sizeof(size)) {
          size = transformSize(*reinterpret_cast<decltype(size) *>(data.data()));
          data.erase(data.begin(),
                     data.begin() + sizeof(size)); // we need a better way than this, this is slow
        }

        if (size) {
          if (size < data.size()) {
            std::string_view view(data);
            m_fn(view.substr(0, size));
            data = view.substr(size);
          } else {
            break;
          }
        }
      }

      if (rc < m_buf.size()) break;
    }
  }

private:
  std::array<char, READ_SIZE> m_buf;
  std::string data;
  uint32_t size = 0;
  ByteOrder m_bo;
  Handler m_fn;
};

struct Version {
  static constexpr const auto key = "version";
  struct Request {};
  struct Response {};
};

namespace commands {
struct FocusTab {
  static constexpr const auto key = "focus-tab";
  struct Request {
    std::string windowId;
    std::string tabId;
  };
  struct Response {};
};

}; // namespace commands

using ExtensionRpcSchema = ipc::RpcSchema<commands::FocusTab>;

static void sendExtensionCommand(std::string_view message) {
  uint32_t size = message.size();
  std::cout.write(reinterpret_cast<char *>(&size), sizeof(size));
  std::cout.write(message.data(), message.size());
  std::cout.flush();
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

  std::println(std::cerr, "Listening for messages...");

  int sock = -1;

  Frame extensionFrame(Frame::ByteOrder::Host, [](std::string_view message) {
    /*
if (const auto error = glz::read_json(msg, message)) {
std::println(std::cerr, "\"{}\"", message);
std::println(std::cerr, "Failed to parse json: {}", glz::format_error(error));
}

std::println(std::cerr, "Processing message: {}", message);
*/
  });

  Frame vicinaeFrame(Frame::ByteOrder::Network, [](std::string_view message) {
    // focus tab
    ipc::RpcClient<ExtensionRpcSchema> client;
    const auto json = client.request<commands::FocusTab>({.windowId = "", .tabId = "some"});
    sendExtensionCommand(json);
  });

  auto fds = std::array{pollfd(STDIN_FILENO, POLLIN), pollfd(sock, POLLIN)};

  assert(fds.size() == 2);

  while (true) {
    int ready = poll(fds.data(), fds.size(), 1000);

    // read extension stuff
    if (fds[0].revents & POLLIN) { extensionFrame.readPart(fds[0].fd); }

    // take vicinae requests/response
    if (fds[1].revents & POLLIN) { vicinaeFrame.readPart(fds[1].fd); }
  }

  std::println(std::cerr, "Got EOF, exiting...");

  /*
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
                                                    */
}
