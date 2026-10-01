#pragma once
#include <cstddef>
#include <span>

namespace Crypto::detail {

bool randomBytes(std::span<std::byte> out);

bool deriveKey(std::span<const std::byte> ikm, std::span<const std::byte> salt,
               std::span<const std::byte> info, std::span<std::byte> out);

bool gcmEncrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> plaintext, std::span<std::byte> ciphertextOut,
                std::span<std::byte> tagOut);

bool gcmDecrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> ciphertext, std::span<const std::byte> tag,
                std::span<std::byte> plaintextOut);

} // namespace Crypto::detail
