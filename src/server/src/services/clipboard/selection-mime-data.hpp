#pragma once
#include <algorithm>
#include <filesystem>
#include <QImage>
#include <QMimeData>
#include <QUrl>
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
  DragAndDropSelectionMimeData(ClipboardSelection selection) {
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
      setData(offer.mimeType, std::move(offer.data));
    }
  }

  QStringList formats() const override { return m_formats; }

  QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override {
    qDebug() << "retrieve format" << mimetype << preferredType;

    if (m_imageMimeType && mimetype == Clipboard::URI_LIST) {
      auto path = QString::fromStdString(Omnicast::runtimeDir() / "vicinae-drag-image.png");

      if (ensureImage(path)) {
        auto file = QUrl::fromLocalFile(path);
        if (preferredType == QMetaType::fromType<QVariantList>()) return QVariantList{file};
        return file;
      }
    }

    return QMimeData::retrieveData(mimetype, preferredType);
  }

protected:
  bool ensureImage(const QString &path) const {
    if (!m_imageMimeType) return false;
    if (imageLoaded) return true;

    auto img = QImage::fromData(data(*m_imageMimeType));
    if (!img.save(path, "PNG")) {
      qWarning() << "[DND] failed to save image data as PNG";
      return false;
    }
    return imageLoaded = true;
  }

private:
  mutable bool imageLoaded = false;
  std::optional<QString> m_imageMimeType;
  QStringList m_formats;
};
