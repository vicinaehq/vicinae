#include "services/clipboard/clipboard-encrypter.hpp"
#include <qstringview.h>
#include <qt6keychain/keychain.h>
#include "vicinae.hpp"
#include "crypto/aes-gcm.hpp"
#include <cstddef>
#include <span>

static const QString KEYCHAIN_ENCRYPTION_KEY_NAME = "clipboard-data-key";

static std::span<const std::byte> asBytes(const QByteArray &b) {
  return {reinterpret_cast<const std::byte *>(b.constData()), static_cast<size_t>(b.size())};
}

static QByteArray toQByteArray(std::span<const std::byte> b) {
  return {reinterpret_cast<const char *>(b.data()), static_cast<qsizetype>(b.size())};
}

void ClipboardEncrypter::loadKey() {
  using namespace QKeychain;

  auto readJob = new ReadPasswordJob(Omnicast::APP_ID);

  readJob->setKey(KEYCHAIN_ENCRYPTION_KEY_NAME);
  readJob->start();

  connect(readJob, &ReadPasswordJob::finished, this, [this, readJob](Job *) {
    if (readJob->error() == QKeychain::NoError) {
      m_key = readJob->binaryData();
      return;
    }

    auto writeJob = new QKeychain::WritePasswordJob(Omnicast::APP_ID);
    auto generated = Crypto::AES256GCM::generateKey();
    QByteArray keyData(reinterpret_cast<const char *>(generated.data()), generated.size());

    writeJob->setKey(KEYCHAIN_ENCRYPTION_KEY_NAME);
    writeJob->setBinaryData(keyData);
    writeJob->start();

    connect(writeJob, &WritePasswordJob::finished, this, [this, keyData, writeJob]() {
      if (writeJob->error() == QKeychain::NoError) {
        m_key = keyData;
        return;
      }

      qCritical() << "Failed to write encryption key to keychain" << writeJob->errorString();
      m_keychainError = writeJob->errorString();
    });
  });
}

ClipboardEncrypter::EncryptResult ClipboardEncrypter::encrypt(const QByteArray &plain) const {
  if (!m_keychainError.isEmpty()) return std::unexpected(m_keychainError);

  auto encrypted = Crypto::AES256GCM::encrypt(asBytes(plain), asBytes(m_key));
  if (!encrypted) return std::unexpected("Encryption failed");
  return toQByteArray(*encrypted);
}

ClipboardEncrypter::DecryptResult ClipboardEncrypter::decrypt(const QByteArray &encrypted) const {
  if (!m_keychainError.isEmpty()) return std::unexpected(m_keychainError);

  auto decrypted = Crypto::AES256GCM::decrypt(asBytes(encrypted), asBytes(m_key));
  if (!decrypted) return std::unexpected("Decryption failed");
  return toQByteArray(*decrypted);
}
