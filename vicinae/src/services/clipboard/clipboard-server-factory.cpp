#include "services/clipboard/clipboard-server-factory.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "services/clipboard/wlr-clipboard-server.hpp"
#include "services/clipboard/dummy-clipboard-server.hpp"

AbstractClipboardServer *ClipboardServerFactory::create(ClipboardServerType type, QObject *parent) const {
  switch (type) {
  case WlrootsDataControlClipboardServer:
    return new WlrClipboardServer();
  default:
    break;
  }

  return new DummyClipboardServer;
}

AbstractClipboardServer *ClipboardServerFactory::createFirstActivatable(QObject *parent) const {
  for (int i = 0; i != InvalidClipboardServer; ++i) {
    auto server = create(static_cast<ClipboardServerType>(i), parent);

    if (server->isActivatable()) { return server; }

    server->deleteLater();
  }

  return new DummyClipboardServer;
}
