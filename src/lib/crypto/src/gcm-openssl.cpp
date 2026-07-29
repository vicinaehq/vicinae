#include "gcm-backend.hpp"
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>

namespace Crypto::detail {

bool randomBytes(std::span<std::byte> out) {
  return RAND_bytes(reinterpret_cast<unsigned char *>(out.data()), static_cast<int>(out.size())) == 1;
}

bool deriveKey(std::span<const std::byte> ikm, std::span<const std::byte> salt,
               std::span<const std::byte> info, std::span<std::byte> out) {
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
  if (!ctx) return false;

  auto u8 = [](auto p) { return reinterpret_cast<const unsigned char *>(p); };

  bool ok = EVP_PKEY_derive_init(ctx) == 1 && EVP_PKEY_CTX_set_hkdf_md(ctx, EVP_sha256()) == 1 &&
            EVP_PKEY_CTX_set1_hkdf_key(ctx, u8(ikm.data()), static_cast<int>(ikm.size())) == 1;
  if (ok && !salt.empty())
    ok = EVP_PKEY_CTX_set1_hkdf_salt(ctx, u8(salt.data()), static_cast<int>(salt.size())) == 1;
  if (ok && !info.empty())
    ok = EVP_PKEY_CTX_add1_hkdf_info(ctx, u8(info.data()), static_cast<int>(info.size())) == 1;

  size_t outlen = out.size();
  ok = ok && EVP_PKEY_derive(ctx, reinterpret_cast<unsigned char *>(out.data()), &outlen) == 1 &&
       outlen == out.size();

  EVP_PKEY_CTX_free(ctx);
  return ok;
}

bool gcmEncrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> plaintext, std::span<std::byte> ciphertextOut,
                std::span<std::byte> tagOut) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return false;

  auto u8 = [](auto p) { return reinterpret_cast<const unsigned char *>(p); };
  auto u8m = [](auto p) { return reinterpret_cast<unsigned char *>(p); };

  bool ok = EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, u8(key.data()), u8(iv.data())) == 1;

  int len = 0;
  ok = ok && EVP_EncryptUpdate(ctx, u8m(ciphertextOut.data()), &len, u8(plaintext.data()),
                               static_cast<int>(plaintext.size())) == 1;
  ok = ok && EVP_EncryptFinal_ex(ctx, u8m(ciphertextOut.data()) + len, &len) == 1;
  ok = ok &&
       EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(tagOut.size()), tagOut.data()) == 1;

  EVP_CIPHER_CTX_free(ctx);
  return ok;
}

bool gcmDecrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> ciphertext, std::span<const std::byte> tag,
                std::span<std::byte> plaintextOut) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) return false;

  auto u8 = [](auto p) { return reinterpret_cast<const unsigned char *>(p); };
  auto u8m = [](auto p) { return reinterpret_cast<unsigned char *>(p); };

  bool ok = EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, u8(key.data()), u8(iv.data())) == 1;

  int len = 0;
  ok = ok && EVP_DecryptUpdate(ctx, u8m(plaintextOut.data()), &len, u8(ciphertext.data()),
                               static_cast<int>(ciphertext.size())) == 1;
  ok = ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(tag.size()),
                                 const_cast<std::byte *>(tag.data())) == 1;
  ok = ok && EVP_DecryptFinal_ex(ctx, u8m(plaintextOut.data()) + len, &len) == 1;

  EVP_CIPHER_CTX_free(ctx);
  return ok;
}

} // namespace Crypto::detail
