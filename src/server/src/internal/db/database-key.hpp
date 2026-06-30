#pragma once
#include "key.hpp"
#include <filesystem>
#include <initializer_list>
#include <optional>

namespace db {

// Migrates the given databases to match `enabled` and returns the key to open them with
// (nullopt when off). Aborts via qFatal on any keychain/migration failure.
std::optional<EncryptionKey>
prepareDatabaseEncryption(bool enabled, std::initializer_list<std::filesystem::path> databases);

} // namespace db
