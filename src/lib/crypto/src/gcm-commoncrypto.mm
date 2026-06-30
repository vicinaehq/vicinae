#include <CommonCrypto/CommonCryptor.h>
#include <CommonCrypto/CommonCryptorSPI.h>
#include <CommonCrypto/CommonRandom.h>
#include "gcm-backend.hpp"

namespace Crypto::detail {

bool randomBytes(std::span<std::byte> out) {
    return CCRandomGenerateBytes(out.data(), out.size()) == kCCSuccess;
}

bool gcmEncrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> plaintext, std::span<std::byte> ciphertextOut,
                std::span<std::byte> tagOut) {
    return CCCryptorGCMOneshotEncrypt(kCCAlgorithmAES, key.data(), key.size(), iv.data(), iv.size(),
                                      nullptr, 0, plaintext.data(), plaintext.size(),
                                      ciphertextOut.data(), tagOut.data(),
                                      tagOut.size()) == kCCSuccess;
}

bool gcmDecrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> ciphertext, std::span<const std::byte> tag,
                std::span<std::byte> plaintextOut) {
    return CCCryptorGCMOneshotDecrypt(kCCAlgorithmAES, key.data(), key.size(), iv.data(), iv.size(),
                                      nullptr, 0, ciphertext.data(), ciphertext.size(),
                                      plaintextOut.data(), tag.data(), tag.size()) == kCCSuccess;
}

}  // namespace Crypto::detail
