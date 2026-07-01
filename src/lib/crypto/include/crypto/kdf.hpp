#pragma once
#include <array>
#include <cstddef>
#include <span>
#include <string_view>

namespace Crypto {

inline constexpr std::size_t SUBKEY_SIZE = 32;

// Derives a 32-byte purpose-specific subkey from a master key via HKDF-SHA256.
// Distinct labels yield independent keys; the same (master, label) is deterministic.
std::array<std::byte, SUBKEY_SIZE> deriveKey(std::span<const std::byte> master, std::string_view label);

} // namespace Crypto
