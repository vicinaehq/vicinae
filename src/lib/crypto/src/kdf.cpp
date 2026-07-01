#include "crypto/kdf.hpp"
#include "gcm-backend.hpp"

namespace Crypto {

std::array<std::byte, SUBKEY_SIZE> deriveKey(std::span<const std::byte> master, std::string_view label) {
  std::array<std::byte, SUBKEY_SIZE> out{};
  auto info = std::as_bytes(std::span<const char>(label.data(), label.size()));
  detail::deriveKey(master, {}, info, out);
  return out;
}

} // namespace Crypto
