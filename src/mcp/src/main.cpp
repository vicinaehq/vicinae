#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <print>
#include <string>
#include <thread>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLocalSocket>
#include "common/common.hpp"
#include "common/frame-parser.hpp"
#include "generated/mcp-ipc-client.hpp"

#ifdef _WIN32
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#endif

static constexpr int CONNECT_TIMEOUT_MS = 3000;

class LocalSocketTransport : public ipc::AbstractTransport {
public:
  explicit LocalSocketTransport(QLocalSocket &socket) : m_socket(socket) {}

  void send(std::string_view data) override {
    const std::uint32_t size = data.size();
    m_socket.write(reinterpret_cast<const char *>(&size), sizeof(size));
    m_socket.write(data.data(), static_cast<qint64>(data.size()));
  }

private:
  QLocalSocket &m_socket;
};

int main(int ac, char **av) {
#ifdef _WIN32
  _setmode(_fileno(stdout), _O_BINARY);
#endif

  QCoreApplication app(ac, av);

  // agents may launch us with a stripped environment, so Vicinae passes its socket explicitly
  QCommandLineParser parser;
  const QCommandLineOption socketOption("socket", "Vicinae server socket to connect to.", "name");
  parser.addOption(socketOption);
  parser.process(app);

  QLocalSocket socket;
  socket.connectToServer(parser.isSet(socketOption) ? parser.value(socketOption)
                                                    : QString::fromStdString(vicinae::serverSocketName()));

  if (!socket.waitForConnected(CONNECT_TIMEOUT_MS)) {
    std::println(std::cerr, "Failed to connect to Vicinae, is it running? {}",
                 socket.errorString().toStdString());
    return 1;
  }

  LocalSocketTransport transport(socket);
  ipc::RpcTransport rpc(transport);
  ipc::Client client(rpc);

  client.ipc().onMcpmessage([](const std::string &message) { std::cout << message << '\n' << std::flush; });

  vicinae::FrameParser frames;
  frames.setHandler([&](std::string_view message) {
    if (auto res = client.route(message); !res)
      std::println(std::cerr, "Failed to route Vicinae message: {}", res.error());
  });

  QObject::connect(&socket, &QLocalSocket::readyRead, [&] {
    const QByteArray data = socket.readAll();
    frames.feed(std::string_view(data.constData(), static_cast<std::size_t>(data.size())));
  });

  QObject::connect(&socket, &QLocalSocket::disconnected, [] {
    std::println(std::cerr, "The connection to Vicinae was lost");
    QCoreApplication::exit(1);
  });

  const auto send = [&client](std::string message) {
    client.ipc().mcpSend(message, [](auto res) {
      if (!res) std::println(std::cerr, "Failed to relay MCP message: {}", res.error());
    });
  };

  // stdin cannot be watched asynchronously on Windows (anonymous pipe, no overlapped I/O),
  // so a blocking reader thread is used on all platforms
  std::thread([&app, &send] {
    for (std::string line; std::getline(std::cin, line);) {
      if (line.ends_with('\r')) line.pop_back();
      if (line.empty()) continue;
      QMetaObject::invokeMethod(&app, [&send, line = std::move(line)] { send(line); }, Qt::QueuedConnection);
    }

    QMetaObject::invokeMethod(&app, [] { QCoreApplication::quit(); }, Qt::QueuedConnection);
  }).detach();

  const int rc = app.exec();
  std::cout.flush();
  // the reader thread may still be blocked on stdin; skip teardown it could race with
  std::_Exit(rc);
}
