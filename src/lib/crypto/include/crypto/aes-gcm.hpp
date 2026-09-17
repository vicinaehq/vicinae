#pragma once
#include <array>
#include <cstddef>
#include <expected>
#include <optional>
#include <span>
#include <vector>

namespace Crypto::AES256GCM {

inline constexpr std::size_t KEY_SIZE = 32;

enum class DecryptError { InvalidKeySize, DataTooShort, CipherError, AuthFailed };
enum class EncryptError { CipherError };

// nullopt when the system random generator is unavailable; never returns a weak key
std::optional<std::array<std::byte, KEY_SIZE>> generateKey();
std::expected<std::vector<std::byte>, EncryptError> encrypt(std::span<const std::byte> data,
                                                            std::span<const std::byte> key);
std::expected<std::vector<std::byte>, DecryptError> decrypt(std::span<const std::byte> encrypted,
                                                            std::span<const std::byte> key);

} // namespace Crypto::AES256GCM
