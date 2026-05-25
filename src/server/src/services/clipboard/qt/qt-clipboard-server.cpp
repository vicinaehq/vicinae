#include "qt-clipboard-server.hpp"
#include <qlogging.h>
#include <ranges>

namespace {
constexpr const char *CONCEALED_MARKER = "vicinae/concealed";
constexpr const char *PASSWORD_MARKER = "x-kde-passwordManagerHint";
} // namespace

bool AbstractQtClipboardServer::start() {
  auto clip = QGuiApplication::clipboard();
  connect(clip, &QClipboard::dataChanged, this, &AbstractQtClipboardServer::dataChanged);
  return true;
}

bool AbstractQtClipboardServer::stop() {
  disconnect(QGuiApplication::clipboard());
  return true;
}

bool AbstractQtClipboardServer::setClipboardContent(QMimeData *data, const Clipboard::CopyOptions &options) {
  if (options.concealed) { data->setData(CONCEALED_MARKER, "1"); }
  return AbstractClipboardServer::setClipboardContent(data, options);
}

std::optional<ClipboardSelection>
AbstractQtClipboardServer::selectionFromMimeData(const QMimeData *mimeData) {
  if (!mimeData) return ClipboardSelection{};

  if (mimeData->hasFormat(CONCEALED_MARKER)) {
    qInfo() << "Qt clipboard: dropping concealed selection";
    return std::nullopt;
  }

  ClipboardSelection selection;
  selection.isPassword = mimeData->hasFormat(PASSWORD_MARKER);

  if (mimeData->hasImage()) {
    // Prefer image formats in order: PNG > JPEG/JPG > SVG > anything else
    auto formats = mimeData->formats();
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

    if (!selectedFormat.isEmpty()) {
      ClipboardDataOffer offer;
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
      if (!offer.data.isEmpty()) { offer.data += ';'; }
      offer.data += url.toString().toUtf8();
    }
    selection.offers.emplace_back(offer);
  }

  // We also want to index other formats that are not text, image, or legacy X11 target types.

  auto isIndexableFormat = [](const QString &fmt) {
    return !isLegacyContentType(fmt) && !fmt.startsWith("text/") && !fmt.startsWith("image/") &&
           fmt != PASSWORD_MARKER;
  };

  for (const auto &format : mimeData->formats() | std::views::filter(isIndexableFormat)) {
    QByteArray const data = mimeData->data(format);
    selection.offers.emplace_back(ClipboardDataOffer{format, data});
  }

  return selection;
}

void AbstractQtClipboardServer::dataChanged() {
  if (auto selection = selectionFromMimeData(QGuiApplication::clipboard()->mimeData())) {
    emit selectionAdded(*selection);
  }
}

bool AbstractQtClipboardServer::isLegacyContentType(const QString &str) {
  if (str.startsWith("-x") || str.startsWith("-X")) return false;
  return str.isUpper() && !str.contains('/');
}
