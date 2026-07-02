#include <catch2/catch_test_macros.hpp>
#include "crypto/aes-gcm.hpp"
#include "crypto/kdf.hpp"
#include "gcm-backend.hpp"
#include <array>
#include <span>
#include <string>
#include <vector>

using namespace Crypto::AES256GCM;

static std::vector<std::byte> bytes(std::string_view s) {
  std::vector<std::byte> v(s.size());
  for (size_t i = 0; i < s.size(); ++i)
    v[i] = static_cast<std::byte>(s[i]);
  return v;
}

static std::string hex(std::span<const std::byte> b) {
  static constexpr char d[] = "0123456789abcdef";
  std::string out;
  for (std::byte x : b) {
    auto v = std::to_integer<unsigned char>(x);
    out.push_back(d[v >> 4]);
    out.push_back(d[v & 0x0F]);
  }
  return out;
}

static std::vector<std::byte> fromHex(std::string_view h) {
  auto nib = [](char c) -> int { return c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10; };
  std::vector<std::byte> v;
  for (size_t i = 0; i + 1 < h.size(); i += 2)
    v.push_back(static_cast<std::byte>((nib(h[i]) << 4) | nib(h[i + 1])));
  return v;
}

TEST_CASE("generateKey returns 32 bytes") { REQUIRE(generateKey().size() == 32); }

TEST_CASE("round trip recovers plaintext") {
  auto key = generateKey();
  auto plain = bytes("the quick brown fox jumps over the lazy dog");
  auto enc = encrypt(plain, key);
  REQUIRE(enc.has_value());
  auto dec = decrypt(*enc, key);
  REQUIRE(dec.has_value());
  REQUIRE(*dec == plain);
}

TEST_CASE("empty plaintext round trips") {
  auto key = generateKey();
  auto enc = encrypt({}, key);
  REQUIRE(enc.has_value());
  REQUIRE(enc->size() == 12 + 16); // iv + tag, no ciphertext
  auto dec = decrypt(*enc, key);
  REQUIRE(dec.has_value());
  REQUIRE(dec->empty());
}

TEST_CASE("wrong key fails authentication") {
  auto enc = encrypt(bytes("secret"), generateKey());
  REQUIRE(enc.has_value());
  auto dec = decrypt(*enc, generateKey());
  REQUIRE_FALSE(dec.has_value());
  REQUIRE(dec.error() == DecryptError::AuthFailed);
}

TEST_CASE("tampered ciphertext is rejected") {
  auto key = generateKey();
  auto enc = encrypt(bytes("secret data here"), key);
  REQUIRE(enc.has_value());
  (*enc)[15] ^= std::byte{0x01};
  REQUIRE_FALSE(decrypt(*enc, key).has_value());
}

TEST_CASE("tampered tag is rejected") {
  auto key = generateKey();
  auto enc = encrypt(bytes("secret"), key);
  REQUIRE(enc.has_value());
  enc->back() ^= std::byte{0x01};
  REQUIRE_FALSE(decrypt(*enc, key).has_value());
}

TEST_CASE("wrong key size reports InvalidKeySize") {
  std::vector<std::byte> shortKey(16);
  auto r = decrypt(std::vector<std::byte>(40), shortKey);
  REQUIRE_FALSE(r.has_value());
  REQUIRE(r.error() == DecryptError::InvalidKeySize);
}

TEST_CASE("too-short input reports DataTooShort") {
  auto r = decrypt(std::vector<std::byte>(10), generateKey());
  REQUIRE_FALSE(r.has_value());
  REQUIRE(r.error() == DecryptError::DataTooShort);
}

TEST_CASE("random IV yields distinct ciphertexts") {
  auto key = generateKey();
  auto a = encrypt(bytes("same plaintext"), key);
  auto b = encrypt(bytes("same plaintext"), key);
  REQUIRE(a.has_value());
  REQUIRE(b.has_value());
  REQUIRE(*a != *b);
}

// GCM spec (McGrew & Viega) AES-256 Test Case 14 — pins the raw cipher to the standard,
// so the OpenSSL and CommonCrypto backends must produce identical output.
TEST_CASE("known answer vector") {
  std::array<std::byte, 32> key{};
  std::array<std::byte, 12> iv{};
  std::array<std::byte, 16> pt{};
  std::array<std::byte, 16> ct{};
  std::array<std::byte, 16> tag{};

  REQUIRE(Crypto::detail::gcmEncrypt(key, iv, pt, ct, tag));
  REQUIRE(hex(ct) == "cea7403d4d606b6e074ec5d3baf39d18");
  REQUIRE(hex(tag) == "d0d1c8a799996bf0265b98b5d48ab919");

  std::array<std::byte, 16> out{};
  REQUIRE(Crypto::detail::gcmDecrypt(key, iv, ct, tag, out));
  REQUIRE(out == pt);

  tag[0] = std::byte{0xFF};
  REQUIRE_FALSE(Crypto::detail::gcmDecrypt(key, iv, ct, tag, out));
}

// RFC 5869 test case 1 — pins HKDF-SHA256 to the standard across both backends.
TEST_CASE("HKDF known answer vector") {
  std::vector<std::byte> ikm(22, std::byte{0x0b});
  auto salt = fromHex("000102030405060708090a0b0c");
  auto info = fromHex("f0f1f2f3f4f5f6f7f8f9");
  std::array<std::byte, 42> okm{};

  REQUIRE(Crypto::detail::deriveKey(ikm, salt, info, okm));
  REQUIRE(hex(okm) == "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56ecc4c5bf34007208d5b887185865");
}

TEST_CASE("deriveKey is deterministic and domain-separated") {
  auto master = generateKey();
  auto db1 = Crypto::deriveKey(master, "vicinae-db");
  auto db2 = Crypto::deriveKey(master, "vicinae-db");
  auto clip = Crypto::deriveKey(master, "vicinae-clipboard");

  REQUIRE(db1.has_value());
  REQUIRE(db2.has_value());
  REQUIRE(clip.has_value());
  REQUIRE(*db1 == *db2);
  REQUIRE(*db1 != *clip);
  REQUIRE(db1->size() == 32);
}
