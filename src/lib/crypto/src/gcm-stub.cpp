#include "gcm-backend.hpp"
#include <algorithm>

namespace Crypto::detail {

bool randomBytes(std::span<std::byte> out) {
  std::ranges::fill(out, std::byte{0});
  return false;
}

bool deriveKey(std::span<const std::byte> ikm, std::span<const std::byte> salt,
               std::span<const std::byte> info, std::span<std::byte> out) {
  std::ranges::fill(out, std::byte{0});
  return false;
}

bool gcmEncrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> plaintext, std::span<std::byte> ciphertextOut,
                std::span<std::byte> tagOut) {
  return false;
}

bool gcmDecrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> ciphertext, std::span<const std::byte> tag,
                std::span<std::byte> plaintextOut) {
  return false;
}

} // namespace Crypto::detail
