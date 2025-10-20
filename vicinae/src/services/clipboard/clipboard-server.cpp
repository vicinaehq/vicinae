#include "clipboard-server.hpp"
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <ranges>

ClipboardSelection AbstractClipboardServer::convertQMimeDataToSelection(const QMimeData *mimeData) {
  ClipboardSelection selection;

  if (!mimeData) return selection;

  QString preferredImageFormat;
  for (const auto &format : mimeData->formats()) {
    if (format.startsWith("image/")) {
      if (format == "image/png") {
        preferredImageFormat = format;
        break;
      }
      if (preferredImageFormat.isEmpty()) {
        preferredImageFormat = format;
      }
    }
  }

  if (!preferredImageFormat.isEmpty()) {
    ClipboardDataOffer offer;
    offer.mimeType = preferredImageFormat;
    offer.data = mimeData->data(preferredImageFormat);
    selection.offers.emplace_back(offer);
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

  auto isIndexableFormat = [](const QString &fmt) {
    return !isLegacyContentType(fmt) && !fmt.startsWith("text/") && !fmt.startsWith("image/");
  };

  for (const auto &format : mimeData->formats() | std::views::filter(isIndexableFormat)) {
    QByteArray data = mimeData->data(format);
    selection.offers.emplace_back(ClipboardDataOffer{format, data});
  }

  return selection;
}

bool AbstractClipboardServer::isLegacyContentType(const QString &str) {
  if (str.startsWith("-x") || str.startsWith("-X")) return false;
  return str.isUpper() && !str.contains('/');
}
