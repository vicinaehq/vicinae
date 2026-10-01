#pragma once

#include <QtGui/qwindowsmimeconverter.h>

// image/png <-> "PNG" as raw bytes
// CF_DIB is rendered only when a consumer asks for it
class WindowsImageMime : public QWindowsMimeConverter {
public:
  WindowsImageMime();

  bool canConvertFromMime(const FORMATETC &formatetc, const QMimeData *mimeData) const override;
  bool convertFromMime(const FORMATETC &formatetc, const QMimeData *mimeData,
                       STGMEDIUM *pmedium) const override;
  QList<FORMATETC> formatsForMime(const QString &mimeType, const QMimeData *mimeData) const override;
  bool canConvertToMime(const QString &mimeType, IDataObject *pDataObj) const override;
  QVariant convertToMime(const QString &mimeType, IDataObject *pDataObj,
                         QMetaType preferredType) const override;
  QString mimeForFormat(const FORMATETC &formatetc) const override;

private:
  static constexpr const char *PNG_MIME = "image/png";

  const unsigned m_pngFormat;
};
