#include "qt-clipboard-server.hpp"

bool AbstractQtClipboardServer::start() {
  auto clip = QApplication::clipboard();
  connect(clip, &QClipboard::dataChanged, this, &AbstractQtClipboardServer::dataChanged);
  return true;
}

bool AbstractQtClipboardServer::stop() {
  disconnect(QApplication::clipboard());
  return true;
}

void AbstractQtClipboardServer::dataChanged() {
  auto clip = QApplication::clipboard();
  auto mimeData = clip->mimeData();

  ClipboardSelection selection = convertQMimeDataToSelection(mimeData);

  emit selectionAdded(selection);
}
