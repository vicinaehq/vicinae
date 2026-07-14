#include <QImage>
#include "clipboard-mime.hpp"
#include <QUrl>
#include <QBuffer>
#include <qlogging.h>
#include <ranges>

static bool isLegacyContentType(const QString &str) {
  if (str.startsWith("-x") || str.startsWith("-X")) return false;
  return str.isUpper() && !str.contains('/');
}

namespace Clipboard {

std::optional<ClipboardSelection> selectionFromMimeData(const QMimeData *mimeData) {
  if (!mimeData) return std::nullopt;

  if (mimeData->hasFormat(Clipboard::CONCEALED_MIME_TYPE)) {
    qInfo() << "Clipboard: dropping concealed selection";
    return std::nullopt;
  }

  auto formats = mimeData->formats();

  ClipboardSelection selection;
  selection.isPassword = mimeData->hasFormat(Clipboard::PASSWORD_HINT_MIME_TYPE);
  selection.offers.reserve(formats.size() + 3);

  if (mimeData->hasImage()) {
    // Prefer image formats in order: PNG > JPEG/JPG > SVG > anything else
    QString selectedFormat;

    if (formats.contains("image/png")) {
      selectedFormat = "image/png";
    } else if (formats.contains("image/jpeg")) {
      selectedFormat = "image/jpeg";
    } else if (formats.contains("image/jpg")) {
      selectedFormat = "image/jpg";
    } else if (formats.contains("image/svg+xml")) {
      selectedFormat = "image/svg+xml";
    } else {
      // Find any other image format
      for (const auto &format : formats) {
        if (format.startsWith("image/")) {
          selectedFormat = format;
          break;
        }
      }
    }

    ClipboardDataOffer offer;

    if (selectedFormat.isEmpty()) {
      auto image = qvariant_cast<QImage>(mimeData->imageData());
      if (!image.isNull()) {
        QBuffer buf(&offer.data);
        image.save(&buf, "PNG");
        offer.mimeType = "image/png";
        selection.offers.emplace_back(offer);
      }
    } else {
      offer.mimeType = selectedFormat;
      offer.data = mimeData->data(selectedFormat);
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
      if (!offer.data.isEmpty()) { offer.data += "\r\n"; }
      offer.data += url.toString().toUtf8();
    }
    selection.offers.emplace_back(std::move(offer));
  }

  // We also want to index other formats that are not text, image, or legacy X11 target types.

  auto isIndexableFormat = [](const QString &fmt) {
#ifdef Q_OS_WIN
    // Unmapped native formats; reading one forces delayed rendering in the source app
    if (fmt.startsWith(Clipboard::WIN_NATIVE_MIME_PREFIX)) return false;
#endif
    // Qt-internal synthetic format
    if (fmt == "application/x-qt-image") return false;
    return !isLegacyContentType(fmt) && !fmt.startsWith("text/") && !fmt.startsWith("image/") &&
           fmt != Clipboard::PASSWORD_HINT_MIME_TYPE;
  };

  for (const auto &format : formats | std::views::filter(isIndexableFormat)) {
    QByteArray const data = mimeData->data(format);
    selection.offers.emplace_back(ClipboardDataOffer{format, data});
  }

  return selection;
}

} // namespace Clipboard
