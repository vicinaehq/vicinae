#pragma once
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include "key.hpp"

namespace db {

enum class CipherState { Plaintext, Encrypted };

std::optional<CipherState> detectCipherState(const std::filesystem::path &path);

// Make sure the sqlite database is encrypted or not, depending on user configuration. If the database is in a
// different state (following a config change) it is migrated using sqlcipher_export.
std::expected<void, std::string> ensureCipherState(const std::filesystem::path &path, bool encrypted,
                                                   KeyView key);

} // namespace db
