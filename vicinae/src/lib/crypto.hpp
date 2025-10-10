#pragma once
#include "expected.hpp"
#include <QByteArray>

namespace Crypto::AES256GCM {
enum class DecryptError {
  InvalidKeySize,
  DataTooShort,
  OpenSslError,
  AuthFailed,
};

enum class EncryptError { OpenSslError };

tl::expected<QByteArray, EncryptError> encrypt(const QByteArray &dta, const QByteArray &ky);
tl::expected<QByteArray, DecryptError> decrypt(const QByteArray &dta, const QByteArray &ky);
QByteArray generateKey();
} // namespace Crypto::AES256GCM

namespace Crypto::UUID {
QString v4();
};
