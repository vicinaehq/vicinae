#include <cstdint>
#include "gcm-backend.hpp"

extern "C" {
bool vicinae_random_bytes(std::uint8_t *out, std::size_t len);
bool vicinae_gcm_encrypt(const std::uint8_t *key, std::size_t keyLen, const std::uint8_t *iv,
                         std::size_t ivLen, const std::uint8_t *pt, std::size_t ptLen, std::uint8_t *ctOut,
                         std::uint8_t *tagOut, std::size_t tagLen);
bool vicinae_gcm_decrypt(const std::uint8_t *key, std::size_t keyLen, const std::uint8_t *iv,
                         std::size_t ivLen, const std::uint8_t *ct, std::size_t ctLen,
                         const std::uint8_t *tag, std::size_t tagLen, std::uint8_t *ptOut);
}

namespace Crypto::detail {

bool randomBytes(std::span<std::byte> out) {
  return vicinae_random_bytes(reinterpret_cast<std::uint8_t *>(out.data()), out.size());
}

bool gcmEncrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> plaintext, std::span<std::byte> ciphertextOut,
                std::span<std::byte> tagOut) {
  return vicinae_gcm_encrypt(reinterpret_cast<const std::uint8_t *>(key.data()), key.size(),
                             reinterpret_cast<const std::uint8_t *>(iv.data()), iv.size(),
                             reinterpret_cast<const std::uint8_t *>(plaintext.data()), plaintext.size(),
                             reinterpret_cast<std::uint8_t *>(ciphertextOut.data()),
                             reinterpret_cast<std::uint8_t *>(tagOut.data()), tagOut.size());
}

bool gcmDecrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> ciphertext, std::span<const std::byte> tag,
                std::span<std::byte> plaintextOut) {
  return vicinae_gcm_decrypt(reinterpret_cast<const std::uint8_t *>(key.data()), key.size(),
                             reinterpret_cast<const std::uint8_t *>(iv.data()), iv.size(),
                             reinterpret_cast<const std::uint8_t *>(ciphertext.data()), ciphertext.size(),
                             reinterpret_cast<const std::uint8_t *>(tag.data()), tag.size(),
                             reinterpret_cast<std::uint8_t *>(plaintextOut.data()));
}

} // namespace Crypto::detail
