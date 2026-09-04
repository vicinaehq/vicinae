#include <windows.h>
#include <ole2.h>
#include <cstring>

#include <QBuffer>
#include <QImage>
#include <QImageWriter>
#include <QMimeData>
#include <QVariant>

#include "windows-image-mime.hpp"

namespace {
constexpr qsizetype BMP_FILE_HEADER_SIZE = 14;

FORMATETC formatEtc(CLIPFORMAT cf) { return {cf, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}; }

bool setHGlobal(STGMEDIUM *pmedium, const QByteArray &data) {
  HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, data.size());
  if (!hglobal) return false;

  void *dst = GlobalLock(hglobal);
  if (!dst) {
    GlobalFree(hglobal);
    return false;
  }
  std::memcpy(dst, data.constData(), data.size());
  GlobalUnlock(hglobal);

  pmedium->tymed = TYMED_HGLOBAL;
  pmedium->hGlobal = hglobal;
  pmedium->pUnkForRelease = nullptr;
  return true;
}

QByteArray getHGlobal(IDataObject *obj, CLIPFORMAT cf) {
  FORMATETC fmt = formatEtc(cf);
  STGMEDIUM medium{};
  if (FAILED(obj->GetData(&fmt, &medium))) return {};

  QByteArray data;
  if (medium.tymed == TYMED_HGLOBAL) {
    if (const void *src = GlobalLock(medium.hGlobal)) {
      data = QByteArray(static_cast<const char *>(src), static_cast<qsizetype>(GlobalSize(medium.hGlobal)));
      GlobalUnlock(medium.hGlobal);
    }
  }
  ReleaseStgMedium(&medium);
  return data;
}

// CF_DIB is a BMP file without its file header
QByteArray dibFromPng(const QByteArray &png) {
  QImage const image = QImage::fromData(png);
  if (image.isNull()) return {};

  QByteArray bmp;
  QBuffer buf(&bmp);
  buf.open(QIODevice::WriteOnly);
  QImageWriter writer(&buf, "BMP");
  if (!writer.write(image)) return {};
  return bmp.mid(BMP_FILE_HEADER_SIZE);
}
} // namespace

WindowsImageMime::WindowsImageMime() : m_pngFormat(RegisterClipboardFormatW(L"PNG")) {}

QString WindowsImageMime::mimeForFormat(const FORMATETC &formatetc) const {
  return formatetc.cfFormat == m_pngFormat ? QString(PNG_MIME) : QString();
}

bool WindowsImageMime::canConvertToMime(const QString &mimeType, IDataObject *pDataObj) const {
  if (mimeType != PNG_MIME) return false;
  FORMATETC fmt = formatEtc(static_cast<CLIPFORMAT>(m_pngFormat));
  return pDataObj->QueryGetData(&fmt) == S_OK;
}

QVariant WindowsImageMime::convertToMime(const QString &mimeType, IDataObject *pDataObj, QMetaType) const {
  if (!canConvertToMime(mimeType, pDataObj)) return {};
  return getHGlobal(pDataObj, static_cast<CLIPFORMAT>(m_pngFormat));
}

QList<FORMATETC> WindowsImageMime::formatsForMime(const QString &mimeType, const QMimeData *) const {
  if (mimeType != PNG_MIME) return {};
  return {formatEtc(static_cast<CLIPFORMAT>(m_pngFormat)), formatEtc(CF_DIB)};
}

bool WindowsImageMime::canConvertFromMime(const FORMATETC &formatetc, const QMimeData *mimeData) const {
  const bool known = formatetc.cfFormat == m_pngFormat || formatetc.cfFormat == CF_DIB;
  return known && (formatetc.tymed & TYMED_HGLOBAL) && mimeData->hasFormat(PNG_MIME);
}

bool WindowsImageMime::convertFromMime(const FORMATETC &formatetc, const QMimeData *mimeData,
                                       STGMEDIUM *pmedium) const {
  if (!canConvertFromMime(formatetc, mimeData)) return false;
  const QByteArray png = mimeData->data(PNG_MIME);
  const QByteArray out = formatetc.cfFormat == CF_DIB ? dibFromPng(png) : png;
  return !out.isEmpty() && setHGlobal(pmedium, out);
}
