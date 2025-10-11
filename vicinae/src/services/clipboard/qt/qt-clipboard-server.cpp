#include "qt-clipboard-server.hpp"
#include <ranges>

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

  // We also want to index other formats that are not text, image, or legacy X11 target types.

  auto isIndexableFormat = [](const QString &fmt) {
    return !isLegacyContentType(fmt) && !fmt.startsWith("text/") && !fmt.startsWith("image/");
  };

  for (const auto &format : mimeData->formats() | std::views::filter(isIndexableFormat)) {
    QByteArray data = mimeData->data(format);
    selection.offers.emplace_back(ClipboardDataOffer{format, data});
  }

  emit selectionAdded(selection);
}

bool AbstractQtClipboardServer::isLegacyContentType(const QString &str) {
  if (str.startsWith("-x") || str.startsWith("-X")) return false;
  return str.isUpper() && !str.contains('/');
}
