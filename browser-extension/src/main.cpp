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

struct BrowserTab {
  int id;
  std::string title;
  std::string url;
  int windowId;
  bool active;
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

  std::println(std::cerr, "Listening for messages...");

  while (readMessage(std::cin, buf)) {
    IncomingExtensionMessage msg;

    if (const auto error = glz::read_json(msg, buf)) {
      std::println(std::cerr, "\"{}\"", buf);
      std::println(std::cerr, "Failed to parse json: {}", glz::format_error(error));
      continue;
    }

    std::println(std::cerr, "Processing message: {}", buf);

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

  std::println(std::cerr, "Got EOF, exiting...");
}
