#pragma once
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <QFile>
#include <QImage>
#include <QMimeData>
#include <QMimeDatabase>
#include <QUrl>
#include <optional>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include "common/clipboard-formats.hpp"
#include "services/clipboard/clipboard-server.hpp"
#include "vicinae.hpp"

/**
 * QMimeData for a saved clipboard selection that is optimized for drag and drop.
 * In particular, this one will automatically write raw image data to a file and
 * advertise a file as one of the transferrable types. This is because unlike regular
 * clipboard copy, drag and drop handlers rarely accept raw image data.
 */
class DragAndDropSelectionMimeData : public QMimeData {
public:
  DragAndDropSelectionMimeData(const ClipboardSelection &selection) {
    auto start = std::chrono::high_resolution_clock::now();
    auto fileIt = std::ranges::find_if(selection.offers, [](const ClipboardDataOffer &offer) {
      return offer.mimeType == Clipboard::URI_LIST;
    });

    bool hasImage = false;

    for (auto &offer : selection.offers) {
      // if we have raw image data and no file attached, then we
      // probably want to convert the image to a file.
      if (!hasImage && offer.mimeType.startsWith("image/")) {
        if (fileIt == selection.offers.end()) {
          hasImage = true;
          m_formats << Clipboard::URI_LIST;
          m_imageMimeType = offer.mimeType;
        }
      }

      m_formats << offer.mimeType;
      setData(offer.mimeType, offer.data);
    }

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start);
    qDebug() << "ctor" << elapsed / 1000 << "seconds";
  }

  QStringList formats() const override { return m_formats; }

  QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override {
    if (m_imageMimeType && mimetype == Clipboard::URI_LIST) {
      if (auto path = ensureImageFile()) { return QVariantList{QUrl::fromLocalFile(*path)}; }
    }

    return QMimeData::retrieveData(mimetype, preferredType);
  }

protected:
  std::optional<QString> ensureImageFile() const {
    if (!m_imageMimeType) return std::nullopt;
    if (m_imageFilePath) return m_imageFilePath;

    const auto bytes = data(*m_imageMimeType);
    const auto suffix = QMimeDatabase().mimeTypeForName(*m_imageMimeType).preferredSuffix();
    auto path = QString::fromStdString((Omnicast::runtimeDir() / "vicinae-drag-image").string());

    if (suffix.isEmpty()) {
      path += ".png";
      if (!QImage::fromData(bytes).save(path, "PNG")) {
        qWarning() << "[DND] failed to save image data as PNG";
        return std::nullopt;
      }
    } else {
      path += '.' + suffix;
      QFile file(path);
      if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size()) {
        qWarning() << "[DND] failed to write drag image to" << path;
        return std::nullopt;
      }
    }

    return m_imageFilePath = path;
  }

private:
  mutable std::optional<QString> m_imageFilePath;
  std::optional<QString> m_imageMimeType;
  QStringList m_formats;
};
