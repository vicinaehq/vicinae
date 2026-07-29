#pragma once
#include <array>
#include <cstddef>
#include <span>

namespace db {

inline constexpr std::size_t KEY_SIZE = 32;
using EncryptionKey = std::array<std::byte, KEY_SIZE>;
using KeyView = std::span<const std::byte, KEY_SIZE>;

} // namespace db
