#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <iostream>
#include <print>
#include <thread>
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

struct NativeNotification {
  std::string method;
  glz::raw_json data;
};

struct ExtensionMessage {
  std::optional<int> id;
  std::string method;
  glz::raw_json data;
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
  auto handleExtensionMessage = [&](std::string_view message) {
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
