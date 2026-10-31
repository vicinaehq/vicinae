#include <cstdint>
#include <cstdio>
#include <cstring>
#include <expected>
#include <functional>
#include <string>
#include <iostream>
#include <print>
#include <thread>
#include <unordered_map>
#include <QCoreApplication>
#include <QLocalSocket>
#include <glaze/glaze.hpp>
#include "common/common.hpp"
#include "generated/browser-ipc-client.hpp"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <unistd.h>
#endif

static constexpr int CONNECT_TIMEOUT_MS = 3000;
static constexpr uint32_t MAX_MESSAGE_SIZE = 64 * 1024 * 1024;

// Incremental parser for the length-prefixed socket stream (readyRead delivers arbitrary chunks)
class Frame {
public:
  using Handler = std::function<void(std::string_view message)>;

  void setHandler(Handler fn) { m_fn = std::move(fn); }

  void feed(std::string_view chunk) {
    m_data.append(chunk);

    for (;;) {
      if (m_size == 0 && m_data.size() >= sizeof(m_size)) {
        std::memcpy(&m_size, m_data.data(), sizeof(m_size));
        m_data.erase(0, sizeof(m_size));
      }

      if (m_size && m_size <= m_data.size()) {
        if (m_fn) m_fn(std::string_view(m_data).substr(0, m_size));
        m_data.erase(0, m_size);
        m_size = 0;
        continue;
      }

      break;
    }
  }

private:
  std::string m_data;
  uint32_t m_size = 0;
  Handler m_fn;
};

static void sendToExtension(std::string_view message) {
  uint32_t size = message.size();
  std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
  std::cout.write(message.data(), message.size());
  std::cout.flush();
}

class LocalSocketTransport : public ipc::AbstractTransport {
public:
  explicit LocalSocketTransport(QLocalSocket &socket) : m_socket(socket) {}

  void send(std::string_view data) override {
    const uint32_t size = data.size();
    m_socket.write(reinterpret_cast<const char *>(&size), sizeof(size));
    m_socket.write(data.data(), data.size());
  }

private:
  QLocalSocket &m_socket;
};

static constexpr glz::opts LENIENT_JSON{.error_on_unknown_keys = false};

struct NativeRequest {
  int id;
  std::string method;
  glz::raw_json data;
};

struct ExtensionMessage {
  std::optional<int> id;
  std::string method;
  glz::raw_json data;
  glz::raw_json result;
  std::optional<std::string> error;
};

// Requests sent to the browser extension through native messaging
class ExtensionPort {
  using ResponseHandler = std::function<void(std::expected<std::string_view, std::string>)>;

public:
  template <typename T> void request(std::string_view method, const auto &data, ipc::Responder<T> respond) {
    std::string dataJson;
    if (auto err = glz::write_json(data, dataJson)) {
      respond(std::unexpected(glz::format_error(err)));
      return;
    }

    const int id = m_nextId++;
    std::string buf;
    if (auto err =
            glz::write_json(NativeRequest{.id = id, .method = std::string{method}, .data = dataJson}, buf)) {
      respond(std::unexpected(glz::format_error(err)));
      return;
    }

    m_pending.insert({id, [respond = std::move(respond)](std::expected<std::string_view, std::string> data) {
                        if (!data) {
                          respond(std::unexpected(std::move(data).error()));
                          return;
                        }

                        if constexpr (std::is_void_v<T>) {
                          respond({});
                        } else {
                          T value;
                          if (auto err = glz::read<LENIENT_JSON>(value, *data)) {
                            respond(std::unexpected(glz::format_error(err)));
                            return;
                          }
                          respond(std::move(value));
                        }
                      }});

    sendToExtension(buf);
  }

  // Returns false if the message is not a response to one of our requests
  bool handleResponse(const ExtensionMessage &msg) {
    if (!msg.method.empty() || !msg.id) return false;

    const auto it = m_pending.find(*msg.id);
    if (it == m_pending.end()) return true;

    const auto handler = std::move(it->second);
    m_pending.erase(it);

    if (msg.error) {
      handler(std::unexpected(*msg.error));
    } else {
      handler(msg.result.str);
    }

    return true;
  }

private:
  std::unordered_map<int, ResponseHandler> m_pending;
  int m_nextId = 0;
};

static bool readExact(char *buf, size_t size) {
  size_t off = 0;

  while (off < size) {
    const size_t rc = std::fread(buf + off, 1, size - off, stdin);
    if (rc == 0) return false;
    off += rc;
  }

  return true;
}

int main(int ac, char **av) {
#ifdef _WIN32
  // CRLF translation corrupts the length-prefixed framing
  _setmode(_fileno(stdin), _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
  const bool tty = _isatty(_fileno(stdin));
#else
  const bool tty = isatty(STDIN_FILENO);
#endif

  if (tty) {
    std::println(
        std::cerr,
        "This executable is meant to be started by the vicinae browser extension using native host messaging "
        "(https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging). Do not "
        "start manually.");
    return 1;
  }

  // Chrome passes extra arguments (--parent-window=, the extension origin); ignore them
  QCoreApplication app(ac, av);

  QLocalSocket socket;
  socket.connectToServer(QString::fromStdString(vicinae::serverSocketName()));

  if (!socket.waitForConnected(CONNECT_TIMEOUT_MS)) {
    std::println(std::cerr, "Failed to connect to vicinae server: {}", socket.errorString().toStdString());
    return 1;
  }

  LocalSocketTransport transport(socket);
  ipc::RpcTransport rpc(transport);
  ipc::Client client(rpc);

  ExtensionPort port;

  client.ipc().onBrowserFocusTab([&](const int32_t &tabId, ipc::Responder<void> respond) {
    port.request("browser/focus-tab", ipc::Ipc_browserFocusTab_Params{.tabId = tabId}, std::move(respond));
  });

  client.ipc().onBrowserCloseTab([&](const int32_t &tabId, ipc::Responder<void> respond) {
    port.request("browser/close-tab", ipc::Ipc_browserCloseTab_Params{.tabId = tabId}, std::move(respond));
  });

  client.ipc().onBrowserGetBookmarks([&](ipc::Responder<std::vector<ipc::BrowserBookmarkInfo>> respond) {
    port.request("browser/get-bookmarks", ipc::Ipc_browserGetBookmarks_Params{}, std::move(respond));
  });

  client.ipc().onBrowserRemoveBookmark([&](const std::string &bookmarkId, ipc::Responder<void> respond) {
    port.request("browser/remove-bookmark", ipc::Ipc_browserRemoveBookmark_Params{.bookmarkId = bookmarkId},
                 std::move(respond));
  });

  client.ipc().onBrowserSearchHistory([&](const std::string &query, const int32_t &maxResults,
                                          ipc::Responder<std::vector<ipc::BrowserHistoryItemInfo>> respond) {
    port.request("browser/search-history",
                 ipc::Ipc_browserSearchHistory_Params{.query = query, .maxResults = maxResults},
                 std::move(respond));
  });

  client.ipc().onBrowserRemoveHistoryUrl([&](const std::string &url, ipc::Responder<void> respond) {
    port.request("browser/remove-history-url", ipc::Ipc_browserRemoveHistoryUrl_Params{.url = url},
                 std::move(respond));
  });

  client.ipc().onBrowserClearHistory([&](ipc::Responder<void> respond) {
    port.request("browser/clear-history", ipc::Ipc_browserClearHistory_Params{}, std::move(respond));
  });

  client.ipc().onBrowserRequestPermission([&](const std::string &permission, ipc::Responder<void> respond) {
    port.request("browser/request-permission",
                 ipc::Ipc_browserRequestPermission_Params{.permission = permission}, std::move(respond));
  });

  // Handle messages from the browser extension (via stdin native messaging)
  auto handleExtensionMessage = [&](std::string_view message) {
    std::println(std::cerr, "got message from extension: {}", message);

    ExtensionMessage msg;
    if (auto err = glz::read<LENIENT_JSON>(msg, message)) {
      std::println(std::cerr, "Failed to parse extension message: {}", glz::format_error(err));
      return;
    }

    if (port.handleResponse(msg)) return;

    if (msg.method == "browser/init") {
      ipc::BrowserInitRequest req;
      // newer extensions may send fields this host does not know about yet
      if (auto err = glz::read<LENIENT_JSON>(req, msg.data.str)) {
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
    } else if (msg.method == "browser/permissions-changed") {
      std::vector<std::string> permissions;
      if (auto err = glz::read_json(permissions, msg.data.str)) {
        std::println(std::cerr, "Failed to parse permissions: {}", glz::format_error(err));
        return;
      }
      client.ipc().browserPermissionsChanged(permissions, [](auto res) {
        if (!res) std::println(std::cerr, "browserPermissionsChanged failed: {}", res.error());
      });
    }
  };

  // Listen for incoming events from vicinae server
  Frame vicinaeFrame;

  vicinaeFrame.setHandler([&](std::string_view message) {
    std::println(std::cerr, "got message from vicinae: {}", message);
    if (auto res = client.route(message); !res) {
      std::println(std::cerr, "Failed to route vicinae message: {}", res.error());
    }
  });

  QObject::connect(&socket, &QLocalSocket::readyRead, [&] {
    const QByteArray data = socket.readAll();
    vicinaeFrame.feed(std::string_view(data.constData(), static_cast<size_t>(data.size())));
  });

  QObject::connect(&socket, &QLocalSocket::disconnected, [] {
    std::println(std::cerr, "The connection to the vicinae daemon broke, exiting...");
    QCoreApplication::quit();
  });

  // stdin cannot be watched asynchronously on Windows (anonymous pipe, no overlapped I/O),
  // so a blocking reader thread is used on all platforms
  std::thread([&app, &handleExtensionMessage] {
    for (;;) {
      uint32_t size = 0;
      if (!readExact(reinterpret_cast<char *>(&size), sizeof(size))) break;
      if (size == 0 || size > MAX_MESSAGE_SIZE) break;

      std::string body(size, '\0');
      if (!readExact(body.data(), body.size())) break;

      QMetaObject::invokeMethod(
          &app, [&handleExtensionMessage, msg = std::move(body)] { handleExtensionMessage(msg); },
          Qt::QueuedConnection);
    }

    QMetaObject::invokeMethod(
        &app,
        [] {
          std::println(std::cerr, "extension input closed, exiting...");
          QCoreApplication::quit();
        },
        Qt::QueuedConnection);
  }).detach();

  std::println(std::cerr, "Listening for messages...");

  const int rc = app.exec();

  std::println(std::cerr, "Native host is exiting.");
  // the reader thread may still be blocked in fread; skip teardown it could race with
  std::_Exit(rc);
}
