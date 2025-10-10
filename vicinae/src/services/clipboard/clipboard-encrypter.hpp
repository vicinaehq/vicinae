#pragma once
#include "expected.hpp"
#include <qobject.h>
#include <qstringview.h>

class ClipboardEncrypter : public QObject {
public:
  using EncryptResult = tl::expected<QByteArray, QString>;
  using DecryptResult = tl::expected<QByteArray, QString>;

  void loadKey();
  EncryptResult encrypt(const QByteArray &plain) const;
  DecryptResult decrypt(const QByteArray &encrypted) const;

private:
  QByteArray m_key;
  QString m_keychainError;
};
