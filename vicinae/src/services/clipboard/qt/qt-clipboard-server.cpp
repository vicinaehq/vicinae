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
  ClipboardSelection selection;
  auto clip = QApplication::clipboard();
  auto mimeData = clip->mimeData();

  if (mimeData->hasImage()) {
    ClipboardDataOffer offer;
    QImage image = qvariant_cast<QImage>(mimeData->imageData());
    QByteArray data;
    QBuffer buf(&data);

    buf.open(QIODevice::WriteOnly);

    if (image.save(&buf, "png")) {
      offer.mimeType = "image/png";
      offer.data = data;
      selection.offers.emplace_back(offer);
    }
  }

  if (mimeData->hasText()) {
    selection.offers.emplace_back(
        ClipboardDataOffer{.mimeType = "text/plain;charset=utf-8", .data = mimeData->text().toUtf8()});
  }

  if (mimeData->hasHtml()) {
    selection.offers.emplace_back(
        ClipboardDataOffer{.mimeType = "text/html", .data = mimeData->html().toUtf8()});
  }

  if (mimeData->hasUrls()) {
    ClipboardDataOffer offer;
    offer.mimeType = "text/uri-list";
    for (const auto &url : mimeData->urls()) {
      if (!offer.data.isEmpty()) { offer.data += ';'; }
      offer.data += url.toString().toUtf8();
    }
    selection.offers.emplace_back(offer);
  }

  emit selectionAdded(selection);
}
