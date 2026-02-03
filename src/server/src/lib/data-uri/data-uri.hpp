#pragma once
#include <QString>
#include <qstringview.h>

struct DataUri {
  QStringView m_mediaType;
  QStringView m_content;
  bool m_base64 = false;

public:
  bool isBase64() const;
  QStringView content() const;
  QStringView mediaType() const;

  /**
   * Decode the content part of the URI, from either base64 or percent encoding.
   */
  QByteArray decodeContent() const;

  /**
   * DataUri does not own any of the data. You need to make sure the passed view remain valid
   * for the entire time this object does.
   */
  DataUri(QStringView uri);
};
