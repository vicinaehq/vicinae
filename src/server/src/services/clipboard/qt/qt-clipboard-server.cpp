#include "qt-clipboard-server.hpp"
#include "services/clipboard/clipboard-mime.hpp"

bool AbstractQtClipboardServer::start() {
  auto clip = QGuiApplication::clipboard();
  connect(clip, &QClipboard::dataChanged, this, &AbstractQtClipboardServer::dataChanged);
  return true;
}

bool AbstractQtClipboardServer::stop() {
  disconnect(QGuiApplication::clipboard());
  return true;
}

void AbstractQtClipboardServer::dataChanged() {
  if (auto selection = Clipboard::selectionFromMimeData(QGuiApplication::clipboard()->mimeData())) {
    emit selectionAdded(*selection);
  }
}
