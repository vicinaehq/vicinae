#include "data-uri.hpp"

QByteArray DataUri::decodeContent() const {
  if (m_base64) return QByteArray::fromBase64(m_content.toUtf8(), QByteArray::Base64UrlEncoding);
  return QByteArray::fromPercentEncoding(m_content.toUtf8());
}

bool DataUri::isBase64() const { return m_base64; }

QStringView DataUri::content() const { return m_content; }

QStringView DataUri::mediaType() const { return m_mediaType; }

DataUri::DataUri(QStringView uri) {
  QStringView s = uri;

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

  if (delimIdx != -1 && delimIdx < contentSepIdx) {
    s = s.sliced(delimIdx + 8);
    m_base64 = true;
  } else {
    s = s.sliced(contentSepIdx + 1);
  }

  m_content = s;
}
