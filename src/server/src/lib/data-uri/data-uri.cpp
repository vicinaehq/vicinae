#include "data-uri.hpp"

QByteArray DataUri::decodeContent() const {
  if (m_base64) return QByteArray::fromBase64(m_content.toUtf8());
  return QByteArray::fromPercentEncoding(m_content.toUtf8());
}

bool DataUri::isBase64() const { return m_base64; }

QStringView DataUri::content() const { return m_content; }

QStringView DataUri::mediaType() const { return m_mediaType; }

DataUri::DataUri(QStringView uri) {
  QStringView s = uri;

  // data:[<media-type>][;base64],<data>
  // where <media-type> can be mime/type;charset=*
  //
  // ex:
  //   data:image/png;base64,
  //   data:image/svg+xml,<svg%20xmlns=
  //   data:image/svg+xml;base64,
  //   data:image/svg+xml;charset=utf-8,
  //   data:image/svg+xml;charset=utf-8;base64,

  if (uri.startsWith(QStringLiteral("data:"))) { s = uri.sliced(5); }

  auto contentSepIdx = s.indexOf(',');
  auto delimIdx = s.indexOf(';');

  auto mediaTypeEnd = 0;

  if (delimIdx != -1 && delimIdx < contentSepIdx) {
    mediaTypeEnd = delimIdx;
    // we need to parse format as well
  } else if (contentSepIdx != -1) {
    mediaTypeEnd = contentSepIdx;
  }

  m_mediaType = s.sliced(0, mediaTypeEnd);

  // Parameters segment exists between first ';' and the ',' before data
  if (delimIdx != -1 && delimIdx < contentSepIdx) {
    auto params = s.sliced(delimIdx, contentSepIdx - delimIdx);
    // base64 may appear alongside other parameters, e.g., ";charset=utf-8;base64"
    m_base64 = params.contains(QLatin1StringView(";base64"), Qt::CaseInsensitive);
  }

  // Content starts after the comma
  if (contentSepIdx != -1) {
    s = s.sliced(contentSepIdx + 1);
  } else {
    s = QStringView();
  }

  m_content = s;
}
