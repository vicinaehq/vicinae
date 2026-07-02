#include "services/clipboard/clipboard-encrypter.hpp"
#include "crypto/aes-gcm.hpp"
#include <cstddef>
#include <span>

static std::span<const std::byte> asBytes(const QByteArray &b) {
  return {reinterpret_cast<const std::byte *>(b.constData()), static_cast<size_t>(b.size())};
}

static QByteArray toQByteArray(std::span<const std::byte> b) {
  return {reinterpret_cast<const char *>(b.data()), static_cast<qsizetype>(b.size())};
}

ClipboardEncrypter::EncryptResult ClipboardEncrypter::encrypt(const QByteArray &plain) const {
  auto encrypted = Crypto::AES256GCM::encrypt(asBytes(plain), asBytes(m_key));
  if (!encrypted) return std::unexpected("Encryption failed");
  return toQByteArray(*encrypted);
}

ClipboardEncrypter::DecryptResult ClipboardEncrypter::decrypt(const QByteArray &encrypted) const {
  auto decrypted = Crypto::AES256GCM::decrypt(asBytes(encrypted), asBytes(m_key));
  if (!decrypted) return std::unexpected("Decryption failed");
  return toQByteArray(*decrypted);
}
