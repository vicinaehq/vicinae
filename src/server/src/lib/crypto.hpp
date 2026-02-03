#pragma once
#include <expected>
#include <QByteArray>

namespace Crypto::AES256GCM {
enum class DecryptError {
  InvalidKeySize,
  DataTooShort,
  OpenSslError,
  AuthFailed,
};

enum class EncryptError { OpenSslError };

std::expected<QByteArray, EncryptError> encrypt(const QByteArray &dta, const QByteArray &ky);
std::expected<QByteArray, DecryptError> decrypt(const QByteArray &dta, const QByteArray &ky);
QByteArray generateKey();
} // namespace Crypto::AES256GCM

namespace Crypto::UUID {
QString v4();
};
