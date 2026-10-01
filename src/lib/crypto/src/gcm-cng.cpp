#include "gcm-backend.hpp"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>
#include <array>

namespace Crypto::detail {

namespace {

constexpr std::size_t SHA256_SIZE = 32;

PUCHAR mut(std::span<const std::byte> s) {
  return reinterpret_cast<PUCHAR>(const_cast<std::byte *>(s.data()));
}
PUCHAR mut(std::span<std::byte> s) { return reinterpret_cast<PUCHAR>(s.data()); }

BCRYPT_ALG_HANDLE openAlgorithm(LPCWSTR id, LPCWSTR chainingMode) {
  BCRYPT_ALG_HANDLE alg = nullptr;
  if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&alg, id, nullptr, 0))) return nullptr;
  if (chainingMode) {
    const auto size = static_cast<ULONG>((wcslen(chainingMode) + 1) * sizeof(wchar_t));
    if (!BCRYPT_SUCCESS(BCryptSetProperty(alg, BCRYPT_CHAINING_MODE,
                                          reinterpret_cast<PUCHAR>(const_cast<LPWSTR>(chainingMode)), size,
                                          0))) {
      BCryptCloseAlgorithmProvider(alg, 0);
      return nullptr;
    }
  }
  return alg;
}

// algorithm handles are thread-safe and cheap to keep for the process lifetime
BCRYPT_ALG_HANDLE gcmAlgorithm() {
  static BCRYPT_ALG_HANDLE alg = openAlgorithm(BCRYPT_AES_ALGORITHM, BCRYPT_CHAIN_MODE_GCM);
  return alg;
}

BCRYPT_ALG_HANDLE hkdfAlgorithm() {
  static BCRYPT_ALG_HANDLE alg = openAlgorithm(BCRYPT_HKDF_ALGORITHM, nullptr);
  return alg;
}

struct KeyHandle {
  BCRYPT_KEY_HANDLE handle = nullptr;
  ~KeyHandle() {
    if (handle) BCryptDestroyKey(handle);
  }
};

bool gcm(bool encrypt, std::span<const std::byte> key, std::span<const std::byte> iv,
         std::span<const std::byte> input, std::span<std::byte> output, PUCHAR tag, std::size_t tagSize) {
  auto *alg = gcmAlgorithm();
  if (!alg || output.size() != input.size()) return false;

  KeyHandle k;
  if (!BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(alg, &k.handle, nullptr, 0, mut(key),
                                                 static_cast<ULONG>(key.size()), 0)))
    return false;

  BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info;
  BCRYPT_INIT_AUTH_MODE_INFO(info);
  info.pbNonce = mut(iv);
  info.cbNonce = static_cast<ULONG>(iv.size());
  info.pbTag = tag;
  info.cbTag = static_cast<ULONG>(tagSize);

  // CNG rejects null buffers even for zero-length input
  UCHAR placeholder = 0;
  PUCHAR in = input.empty() ? &placeholder : mut(input);
  PUCHAR out = output.empty() ? &placeholder : mut(output);
  const auto size = static_cast<ULONG>(input.size());
  ULONG written = 0;

  const NTSTATUS status = encrypt
                              ? BCryptEncrypt(k.handle, in, size, &info, nullptr, 0, out, size, &written, 0)
                              : BCryptDecrypt(k.handle, in, size, &info, nullptr, 0, out, size, &written, 0);

  return BCRYPT_SUCCESS(status) && written == size;
}

} // namespace

bool randomBytes(std::span<std::byte> out) {
  return BCRYPT_SUCCESS(
      BCryptGenRandom(nullptr, mut(out), static_cast<ULONG>(out.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG));
}

bool deriveKey(std::span<const std::byte> ikm, std::span<const std::byte> salt,
               std::span<const std::byte> info, std::span<std::byte> out) {
  auto *alg = hkdfAlgorithm();
  if (!alg) return false;

  KeyHandle k;
  if (!BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(alg, &k.handle, nullptr, 0, mut(ikm),
                                                 static_cast<ULONG>(ikm.size()), 0)))
    return false;

  if (!BCRYPT_SUCCESS(BCryptSetProperty(k.handle, BCRYPT_HKDF_HASH_ALGORITHM,
                                        reinterpret_cast<PUCHAR>(const_cast<LPWSTR>(BCRYPT_SHA256_ALGORITHM)),
                                        sizeof(BCRYPT_SHA256_ALGORITHM), 0)))
    return false;

  // RFC 5869: an absent salt is HashLen zero bytes, which HMAC keys to the same value as an empty key
  std::array<std::byte, SHA256_SIZE> zeroSalt{};
  auto effectiveSalt = salt.empty() ? std::span<const std::byte>(zeroSalt) : salt;
  if (!BCRYPT_SUCCESS(BCryptSetProperty(k.handle, BCRYPT_HKDF_SALT_AND_FINALIZE, mut(effectiveSalt),
                                        static_cast<ULONG>(effectiveSalt.size()), 0)))
    return false;

  BCryptBuffer buffer{static_cast<ULONG>(info.size()), KDF_HKDF_INFO, mut(info)};
  BCryptBufferDesc params{BCRYPTBUFFER_VERSION, 1, &buffer};
  ULONG written = 0;

  const NTSTATUS status = BCryptKeyDerivation(k.handle, info.empty() ? nullptr : &params, mut(out),
                                              static_cast<ULONG>(out.size()), &written, 0);
  return BCRYPT_SUCCESS(status) && written == out.size();
}

bool gcmEncrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> plaintext, std::span<std::byte> ciphertextOut,
                std::span<std::byte> tagOut) {
  return gcm(true, key, iv, plaintext, ciphertextOut, mut(tagOut), tagOut.size());
}

bool gcmDecrypt(std::span<const std::byte> key, std::span<const std::byte> iv,
                std::span<const std::byte> ciphertext, std::span<const std::byte> tag,
                std::span<std::byte> plaintextOut) {
  return gcm(false, key, iv, ciphertext, plaintextOut, mut(tag), tag.size());
}

} // namespace Crypto::detail
