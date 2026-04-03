#include <cassert>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <glaze/glaze.hpp>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <filesystem>
#include "generated/browser-ipc-client.hpp"

#ifdef NDEBUG
#define READ_SIZE 8096
#else
#define READ_SIZE 1
#endif

class Frame {
public:
  using Handler = std::function<void(std::string_view message)>;

  void setHandler(Handler fn) { m_fn = std::move(fn); }

  bool readPart(int fd) {
    const int rc = read(fd, m_buf.data(), m_buf.size());

    if (rc == -1) {
      std::println(std::cerr, "Failed to read fd {}: {}", fd, strerror(errno));
      return false;
    }

    if (rc == 0) { return false; }

    data.append(std::string_view(m_buf.data(), rc));

    for (;;) {
      if (size == 0 && data.size() >= sizeof(size)) {
        size = *reinterpret_cast<decltype(size) *>(data.data());
        data.erase(data.begin(), data.begin() + sizeof(size));
      }

      if (size && size <= data.size()) {
        const std::string_view view(data);
        if (m_fn) m_fn(view.substr(0, size));
        data = view.substr(size);
        size = 0;
        continue;
      }

      break;
    }

    return true;
  }

private:
  std::array<char, READ_SIZE> m_buf;
  std::string data;
  uint32_t size = 0;
  Handler m_fn;
};

static std::filesystem::path socketPath() {
  return std::filesystem::path(getenv("XDG_RUNTIME_DIR")) / "vicinae" / "vicinae.sock";
}

static void sendToExtension(std::string_view message) {
  uint32_t size = message.size();
  std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
  std::cout.write(message.data(), message.size());
  std::cout.flush();
}

class SocketTransport : public ipc::AbstractTransport {
public:
  explicit SocketTransport(int fd) : m_fd(fd) {}

  void send(std::string_view data) override {
    uint32_t size = data.size();
    ::send(m_fd, &size, sizeof(size), 0);
    ::send(m_fd, data.data(), data.size(), 0);
  }

private:
  int m_fd;
};

struct NativeNotification {
  std::string method;
  glz::raw_json data;
};

struct ExtensionMessage {
  std::optional<int> id;
  std::string method;
  glz::raw_json data;
};

static int entrypoint() {
  if (isatty(STDIN_FILENO)) {
    std::println(
        std::cerr,
        "This executable is meant to be started by the vicinae browser extension using native host messaging "
        "(https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging). Do not "
        "start manually.");
    return 1;
  }

  auto path = socketPath().string();
  int const fd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (fd == -1) {
    std::println(std::cerr, "Failed to create socket: {}", strerror(errno));
    return 1;
  }

  struct sockaddr_un addr{.sun_family = AF_UNIX};
  strncpy(addr.sun_path, path.data(), path.size());

  if (::connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
    ::close(fd);
    std::println(std::cerr, "Failed to connect to {}: {}", path, strerror(errno));
    return 1;
  }

  SocketTransport transport(fd);
  ipc::RpcTransport rpc(transport);
  ipc::Client client(rpc);

  // Forward focusTab/closeTab events from vicinae to the browser extension
  auto forwardNotification = [](std::string_view method, const auto &data) {
    std::string dataJson;
    if (auto err = glz::write_json(data, dataJson)) {
      std::println(std::cerr, "Failed to serialize notification data: {}", glz::format_error(err));
      return;
    }

    std::string buf;
    if (auto err =
            glz::write_json(NativeNotification{.method = std::string{method}, .data = dataJson}, buf)) {
      std::println(std::cerr, "Failed to serialize notification: {}", glz::format_error(err));
      return;
    }
    sendToExtension(buf);
  };

  client.ipc().onFocustab(
      [&](const ipc::FocusTabRequest &req) { forwardNotification("browser/focus-tab", req); });

  client.ipc().onClosetab(
      [&](const ipc::CloseTabRequest &req) { forwardNotification("browser/close-tab", req); });

  // Handle messages from the browser extension (via stdin native messaging)
  Frame extensionFrame;

  extensionFrame.setHandler([&](std::string_view message) {
    std::println(std::cerr, "got message from extension: {}", message);

    ExtensionMessage msg;
    if (auto err = glz::read_json(msg, message)) {
      std::println(std::cerr, "Failed to parse extension message: {}", glz::format_error(err));
      return;
    }

    if (msg.method == "browser/init") {
      ipc::BrowserInitRequest req;
      if (auto err = glz::read_json(req, msg.data.str)) {
        std::println(std::cerr, "Failed to parse browserInit: {}", glz::format_error(err));
        return;
      }
      auto initId = msg.id;
      client.ipc().browserInit(req, [initId](auto res) {
        if (!res) {
          std::println(std::cerr, "browserInit failed: {}", res.error());
          return;
        }
        std::println(std::cerr, "browserInit ok");
        if (initId) {
          std::string buf;
          if (auto err = glz::write_json(
                  glz::generic::object_t{{"id", *initId}, {"result", glz::generic::object_t{}}}, buf)) {
            std::println(std::cerr, "Failed to serialize browserInit response: {}", glz::format_error(err));
            return;
          }
          sendToExtension(buf);
        }
      });
    } else if (msg.method == "browser/tabs-changed") {
      std::vector<ipc::BrowserTabInfo> tabs;
      if (auto err = glz::read_json(tabs, msg.data.str)) {
        std::println(std::cerr, "Failed to parse tabs: {}", glz::format_error(err));
        return;
      }
      client.ipc().browserTabsChanged(tabs, [](auto res) {
        if (!res) std::println(std::cerr, "browserTabsChanged failed: {}", res.error());
      });
    }
  });

  // Listen for incoming events from vicinae server
  Frame vicinaeFrame;

  vicinaeFrame.setHandler([&](std::string_view message) {
    std::println(std::cerr, "got message from vicinae: {}", message);
    if (auto res = client.route(message); !res) {
      std::println(std::cerr, "Failed to route vicinae message: {}", res.error());
    }
  });

  auto fds = std::array{pollfd(STDIN_FILENO, POLLIN), pollfd(fd, POLLIN)};

  std::println(std::cerr, "Listening for messages...");

  while (true) {
    const int ready = poll(fds.data(), fds.size(), -1);

    if (ready == -1) {
      std::println(std::cerr, "poll() failed: {} (exiting)", strerror(errno));
      break;
    }

    if (fds[0].revents & (POLLHUP | POLLERR)) {
      std::println(std::cerr, "extension input closed (hangup/error), exiting...");
      break;
    }

    if (fds[1].revents & (POLLHUP | POLLERR)) {
      std::println(std::cerr, "vicinae connection closed (hangup/error), exiting...");
      break;
    }

    if (fds[0].revents & POLLIN) {
      if (!extensionFrame.readPart(fds[0].fd)) {
        std::println(std::cerr, "extension input closed, exiting...");
        break;
      }
    }

    if (fds[1].revents & POLLIN) {
      if (!vicinaeFrame.readPart(fds[1].fd)) {
        std::println(std::cerr, "The connection to the vicinae daemon broke, exiting...");
        break;
      }
    }
  }

  ::close(fd);
  std::println(std::cerr, "Native host is exiting.");

  return 0;
}

int main(int ac, char **av) {
  try {
    return entrypoint();
  } catch (...) { return 1; }
}
