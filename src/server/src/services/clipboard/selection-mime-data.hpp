#include <algorithm>
#include <filesystem>
#include <QImage>
#include <QMimeData>
#include <QUrl>
#include <system_error>
#include "common/clipboard-formats.hpp"
#include "services/clipboard/clipboard-server.hpp"

/**
 * QMimeData for a saved clipboard selection that is optimized for drag and drop.
 * In particular, this one will automatically write raw image data to a file and
 * advertise a file as one of the transferrable types. This is because unlike regular
 * clipboard copy, drag and drop handlers rarely accept raw image data.
 */
class DragAndDropSelectionMimeData : public QMimeData {
public:
  DragAndDropSelectionMimeData(ClipboardSelection selection) : m_selection(std::move(selection)) {
    auto fileIt = std::ranges::find_if(m_selection.offers, [](const ClipboardDataOffer &offer) {
      return offer.mimeType == Clipboard::URI_LIST;
    });

    for (const auto &offer : m_selection.offers) {
      // if we have raw image data and no file attached, then we
      // probably want to convert the image to a file.
      if (offer.mimeType.startsWith("image/")) {
        if (fileIt == m_selection.offers.end()) {
          m_formats << Clipboard::URI_LIST;
          m_imageMimeType = offer.mimeType;
        }
      }
      setData(offer.mimeType, std::move(offer.data));
      m_formats << offer.mimeType;
    }
  }

  QStringList formats() const override { return m_formats; }

  bool hasFormat(const QString &mimetype) const override {
    qDebug() << "has format" << mimetype << m_formats.contains(mimetype);
    return m_formats.contains(mimetype);
  }

  QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override {
    qDebug() << "retrieve format" << mimetype << preferredType;

    if (m_imageMimeType && mimetype == Clipboard::URI_LIST) {
      std::error_code ec;
      auto path = QString::fromStdString(
          (std::filesystem::temp_directory_path(ec) / "vicinae-drag-image.png").string());

      if (!ec && ensureImage(path)) { return QUrl::fromLocalFile(path); }
    }

    return QMimeData::retrieveData(mimetype, preferredType);
  }

protected:
  bool ensureImage(const QString &path) const {
    if (!m_imageMimeType) return false;
    if (imageLoaded) return true;

    auto img = QImage::fromData(data(*m_imageMimeType));
    return imageLoaded = img.save(path, "PNG");
  }

private:
  mutable bool imageLoaded = false;
  ClipboardSelection m_selection;
  std::optional<QString> m_imageMimeType;
  QStringList m_formats;
};
