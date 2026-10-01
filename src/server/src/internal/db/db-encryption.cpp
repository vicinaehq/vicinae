#include <sqlcipher/sqlite3.h>
#include <format>
#include <string>
#include "db-encryption.hpp"

namespace fs = std::filesystem;

namespace db {

namespace {

std::string toHex(KeyView key) {
  static constexpr char HEX_DIGITS[] = "0123456789abcdef";
  std::string out;

  out.reserve(key.size() * 2);

  for (std::byte b : key) {
    auto v = std::to_integer<unsigned char>(b);
    out.push_back(HEX_DIGITS[v >> 4]);
    out.push_back(HEX_DIGITS[v & 0x0F]);
  }

  return out;
}

std::optional<std::string> run(sqlite3 *handle, const std::string &sql) {
  char *errmsg = nullptr;

  if (sqlite3_exec(handle, sql.c_str(), nullptr, nullptr, &errmsg) != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "sqlite error";
    sqlite3_free(errmsg);
    return err;
  }

  return std::nullopt;
}

void removeSidecars(const fs::path &path) {
  std::error_code ec;
  fs::remove(fs::path(path) += "-wal", ec);
  fs::remove(fs::path(path) += "-shm", ec);
}

std::string quoteSqlString(const std::string &value) {
  char *quoted = sqlite3_mprintf("%Q", value.c_str());

  if (!quoted) return "''";

  std::string out = quoted;
  sqlite3_free(quoted);

  return out;
}

} // namespace

std::optional<CipherState> detectCipherState(const fs::path &path) {
  std::error_code ec;

  if (!fs::exists(path, ec)) return std::nullopt;

  sqlite3 *handle = nullptr;

  if (sqlite3_open_v2(path.string().c_str(), &handle, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
    if (handle) sqlite3_close_v2(handle);
    return std::nullopt;
  }

  int const rc = sqlite3_exec(handle, "SELECT count(*) FROM sqlite_master;", nullptr, nullptr, nullptr);

  sqlite3_close_v2(handle);

  if (rc == SQLITE_OK) return CipherState::Plaintext;
  if (rc == SQLITE_NOTADB) return CipherState::Encrypted;
  return std::nullopt;
}

std::expected<void, std::string> ensureCipherState(const fs::path &path, bool encrypted, KeyView key) {
  auto state = detectCipherState(path);

  if (!state) return {};
  if ((*state == CipherState::Encrypted) == encrypted) return {};

  std::error_code ec;

  fs::path tmp = fs::path(path) += ".migrate";
  fs::remove(tmp, ec);

  removeSidecars(tmp);

  sqlite3 *src = nullptr;
  // SQLITE_OPEN_CREATE is required so ATTACH can create the migration target file.
  if (sqlite3_open_v2(path.string().c_str(), &src, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) !=
      SQLITE_OK) {
    std::string err = src ? sqlite3_errmsg(src) : "failed to open database";
    if (src) sqlite3_close_v2(src);
    return std::unexpected(err);
  }

  auto fail = [&](std::string err) -> std::expected<void, std::string> {
    sqlite3_close_v2(src);
    std::error_code e;
    fs::remove(tmp, e);
    removeSidecars(tmp);
    return std::unexpected(std::move(err));
  };

  const std::string hex = toHex(key);

  if (*state == CipherState::Encrypted) {
    if (auto err = run(src, std::format("PRAGMA key = \"x'{}'\";", hex))) {
      return fail(std::format("set key: {}", *err));
    }
  }

  if (auto err = run(src, "SELECT count(*) FROM sqlite_master;")) {
    return fail(std::format("cannot read source database (bad key?): {}", *err));
  }

  run(src, "PRAGMA wal_checkpoint(TRUNCATE);");

  const auto attachKey = encrypted ? std::format("\"x'{}'\"", hex) : std::string("''");

  if (auto err = run(src, std::format("ATTACH DATABASE {} AS migrate KEY {};", quoteSqlString(tmp.string()),
                                      attachKey))) {
    return fail(std::format("attach: {}", *err));
  }

  if (auto err = run(src, "SELECT sqlcipher_export('migrate');")) {
    return fail(std::format("export: {}", *err));
  }

  if (auto err = run(src, "DETACH DATABASE migrate;")) { return fail(std::format("detach: {}", *err)); }

  sqlite3_close_v2(src);

  // The original's stale -wal/-shm would corrupt the swapped-in file.
  removeSidecars(path);
  removeSidecars(tmp);
  fs::rename(tmp, path, ec);

  if (ec) return std::unexpected(std::format("rename migrated database: {}", ec.message()));

  return {};
}

} // namespace db
