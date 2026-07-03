#pragma once
#include "key.hpp"
#include <filesystem>
#include <initializer_list>
#include <optional>

namespace db {

struct EncryptionKeys {
  std::optional<EncryptionKey> database;
  std::optional<EncryptionKey> clipboard;
};

// Reads the master key from the keychain once, derives per-purpose subkeys (HKDF), migrates the
// given databases to match `enabled`, and returns the keys to use (nullopt fields when off).
// Aborts via qFatal on any keychain/migration failure.
EncryptionKeys prepareEncryption(bool enabled, std::initializer_list<std::filesystem::path> databases);

} // namespace db
