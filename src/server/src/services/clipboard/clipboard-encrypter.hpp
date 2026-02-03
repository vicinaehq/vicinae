#pragma once
#include <expected>
#include <qobject.h>
#include <qstringview.h>

class ClipboardEncrypter : public QObject {
public:
  using EncryptResult = std::expected<QByteArray, QString>;
  using DecryptResult = std::expected<QByteArray, QString>;

  void loadKey();
  EncryptResult encrypt(const QByteArray &plain) const;
  DecryptResult decrypt(const QByteArray &encrypted) const;

private:
  QByteArray m_key;
  QString m_keychainError;
};
