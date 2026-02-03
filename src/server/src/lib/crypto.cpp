#include "crypto.hpp"
#include <expected>
#include <openssl/rand.h>
#include <QDebug>
#include <qlcdnumber.h>
#include <quuid.h>

namespace Crypto::AES256GCM {
std::expected<QByteArray, EncryptError> encrypt(const QByteArray &data, const QByteArray &key) {
  QByteArray iv(12, 0);

  RAND_bytes(reinterpret_cast<unsigned char *>(iv.data()), iv.size());

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                     reinterpret_cast<const unsigned char *>(key.constData()),
                     reinterpret_cast<const unsigned char *>(iv.constData()));

  QByteArray ciphertext(data.size(), 0);
  int len;
  if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()), &len,
                        reinterpret_cast<const unsigned char *>(data.constData()), data.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return std::unexpected(EncryptError::OpenSslError);
  }

  int ciphertext_len = len;

  // Finalize encryption
  if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()) + len, &len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return std::unexpected(EncryptError::OpenSslError);
  }
  ciphertext_len += len;

  // Get the authentication tag (16 bytes for GCM)
  QByteArray tag(16, 0);
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return std::unexpected(EncryptError::OpenSslError);
  }

  EVP_CIPHER_CTX_free(ctx);
  ciphertext.resize(ciphertext_len);

  // Return format: IV (12) + Ciphertext + Auth Tag (16)
  return iv + ciphertext + tag;
}

std::expected<QByteArray, DecryptError> decrypt(const QByteArray &encrypted, const QByteArray &key) {
  if (key.size() != 32) { return std::unexpected(DecryptError::InvalidKeySize); }

  if (encrypted.size() < 28) { // IV(12) + tag(16) = minimum 28 bytes
    return std::unexpected(DecryptError::DataTooShort);
  }

  // Extract components
  QByteArray iv = encrypted.left(12);
  QByteArray tag = encrypted.right(16);
  QByteArray ciphertext = encrypted.mid(12, encrypted.size() - 28);

  // Create cipher context
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

  if (!ctx) { return std::unexpected(DecryptError::OpenSslError); }

  // Initialize decryption
  if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                         reinterpret_cast<const unsigned char *>(key.constData()),
                         reinterpret_cast<const unsigned char *>(iv.constData())) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return std::unexpected(DecryptError::OpenSslError);
  }

  // Decrypt
  QByteArray plaintext(ciphertext.size(), 0);
  int len;
  if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(plaintext.data()), &len,
                        reinterpret_cast<const unsigned char *>(ciphertext.constData()),
                        ciphertext.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return std::unexpected(DecryptError::OpenSslError);
  }

  // Set expected tag
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                          const_cast<void *>(reinterpret_cast<const void *>(tag.constData()))) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    return std::unexpected(DecryptError::OpenSslError);
  }

  // Finalize and verify
  int plaintext_len = len;
  if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(plaintext.data()) + len, &len) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    qWarning() << "Authentication failed - data may be corrupted";
    return std::unexpected(DecryptError::AuthFailed);
  }

  plaintext_len += len;
  EVP_CIPHER_CTX_free(ctx);

  plaintext.resize(plaintext_len);
  return plaintext;
}

QByteArray generateKey() {
  QByteArray keyData(32, 0);

  RAND_bytes(reinterpret_cast<unsigned char *>(keyData.data()), keyData.size());

  return keyData;
}

}; // namespace Crypto::AES256GCM

namespace Crypto::UUID {
QString v4() { return QUuid::createUuid().toString(QUuid::WithoutBraces); }
}; // namespace Crypto::UUID
