#pragma once
#include <QByteArray>

namespace Crypto::AES256GCM {
QByteArray encrypt(const QByteArray &dta, const QByteArray &ky);
QByteArray decrypt(const QByteArray &dta, const QByteArray &ky);
QByteArray generateKey();
} // namespace Crypto::AES256GCM

namespace Crypto::UUID {
QString v4();
};
