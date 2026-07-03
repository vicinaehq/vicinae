#pragma once
#include <QByteArray>
#include <QString>
#include <expected>

class ClipboardEncrypter {
public:
  using EncryptResult = std::expected<QByteArray, QString>;
  using DecryptResult = std::expected<QByteArray, QString>;

  explicit ClipboardEncrypter(QByteArray key) : m_key(std::move(key)) {}

  EncryptResult encrypt(const QByteArray &plain) const;
  DecryptResult decrypt(const QByteArray &encrypted) const;

private:
  QByteArray m_key;
};
