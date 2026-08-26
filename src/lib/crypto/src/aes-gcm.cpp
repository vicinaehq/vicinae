#include "crypto/aes-gcm.hpp"
#include "gcm-backend.hpp"
#include <array>

namespace Crypto::AES256GCM {

static constexpr std::size_t IV_SIZE = 12;
static constexpr std::size_t TAG_SIZE = 16;

std::array<std::byte, KEY_SIZE> generateKey() {
  std::array<std::byte, KEY_SIZE> key{};
  detail::randomBytes(key);
  return key;
}

std::expected<std::vector<std::byte>, EncryptError> encrypt(std::span<const std::byte> data,
                                                            std::span<const std::byte> key) {
  if (key.size() != KEY_SIZE) return std::unexpected(EncryptError::CipherError);

  std::vector<std::byte> out(IV_SIZE + data.size() + TAG_SIZE);
  std::span<std::byte> all(out);
  auto iv = all.first(IV_SIZE);
  auto ciphertext = all.subspan(IV_SIZE, data.size());
  auto tag = all.last(TAG_SIZE);

  if (!detail::randomBytes(iv)) return std::unexpected(EncryptError::CipherError);
  if (!detail::gcmEncrypt(key, iv, data, ciphertext, tag)) return std::unexpected(EncryptError::CipherError);

  return out;
}

std::expected<std::vector<std::byte>, DecryptError> decrypt(std::span<const std::byte> encrypted,
                                                            std::span<const std::byte> key) {
  if (key.size() != KEY_SIZE) return std::unexpected(DecryptError::InvalidKeySize);
  if (encrypted.size() < IV_SIZE + TAG_SIZE) return std::unexpected(DecryptError::DataTooShort);

  auto iv = encrypted.first(IV_SIZE);
  auto tag = encrypted.last(TAG_SIZE);
  auto ciphertext = encrypted.subspan(IV_SIZE, encrypted.size() - IV_SIZE - TAG_SIZE);

  std::vector<std::byte> out(ciphertext.size());
  if (!detail::gcmDecrypt(key, iv, ciphertext, tag, out)) return std::unexpected(DecryptError::AuthFailed);

  return out;
}

} // namespace Crypto::AES256GCM
