#pragma once
#include <sqlcipher/sqlite3.h>

#include <QString>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <optional>
#include <qlogging.h>
#include <string>
#include <string_view>
#include <utility>

namespace db {

class Statement {
  sqlite3_stmt *m_stmt = nullptr;
  sqlite3 *m_db = nullptr;
  bool m_valid = true;

  int paramIndex(const char *name) const {
    if (!m_stmt) return 0;
    return sqlite3_bind_parameter_index(m_stmt, name);
  }

public:
  Statement() : m_valid(false) {}

  explicit operator bool() const { return m_valid; }

  Statement(sqlite3_stmt *stmt, sqlite3 *db) : m_stmt(stmt), m_db(db) {}

  ~Statement() {
    if (m_stmt) sqlite3_finalize(m_stmt);
  }

  Statement(const Statement &) = delete;
  Statement &operator=(const Statement &) = delete;

  Statement(Statement &&other) noexcept : m_stmt(other.m_stmt), m_db(other.m_db), m_valid(other.m_valid) {
    other.m_stmt = nullptr;
    other.m_db = nullptr;
    other.m_valid = false;
  }

  Statement &operator=(Statement &&other) noexcept {
    if (this != &other) {
      if (m_stmt) sqlite3_finalize(m_stmt);
      m_stmt = other.m_stmt;
      m_db = other.m_db;
      m_valid = other.m_valid;
      other.m_stmt = nullptr;
      other.m_db = nullptr;
      other.m_valid = false;
    }
    return *this;
  }

  void bind(const char *name, int val) {
    if (int const idx = paramIndex(name)) sqlite3_bind_int(m_stmt, idx, val);
  }

  void bind(const char *name, int64_t val) {
    if (int const idx = paramIndex(name)) sqlite3_bind_int64(m_stmt, idx, val);
  }

  void bind(const char *name, uint64_t val) {
    if (int const idx = paramIndex(name)) sqlite3_bind_int64(m_stmt, idx, static_cast<int64_t>(val));
  }

  void bind(const char *name, double val) {
    if (int const idx = paramIndex(name)) sqlite3_bind_double(m_stmt, idx, val);
  }

  void bind(const char *name, std::string_view val) {
    if (int const idx = paramIndex(name))
      sqlite3_bind_text(m_stmt, idx, val.data(), static_cast<int>(val.size()), SQLITE_TRANSIENT);
  }

  void bind(const char *name, const char *val) { bind(name, std::string_view(val)); }

  void bind(const char *name, const std::string &val) { bind(name, std::string_view(val)); }

  void bind(const char *name, const QString &val) {
    int const idx = paramIndex(name);
    if (!idx) return;
    auto utf8 = val.toUtf8();
    sqlite3_bind_text(m_stmt, idx, utf8.constData(), utf8.size(), SQLITE_TRANSIENT);
  }

  template <typename T> void bind(const char *name, const std::optional<T> &val) {
    if (val) {
      bind(name, *val);
    } else {
      bindNull(name);
    }
  }

  void bindNull(const char *name) {
    if (int const idx = paramIndex(name)) sqlite3_bind_null(m_stmt, idx);
  }

  bool step() {
    if (!m_stmt) return false;
    int const rc = sqlite3_step(m_stmt);
    if (rc == SQLITE_ROW) return true;
    return false;
  }

  bool exec() {
    if (!m_stmt) return false;
    int const rc = sqlite3_step(m_stmt);
    sqlite3_reset(m_stmt);
    sqlite3_clear_bindings(m_stmt);
    if (rc == SQLITE_DONE || rc == SQLITE_ROW) return true;
    return false;
  }

  void reset() {
    if (!m_stmt) return;
    sqlite3_reset(m_stmt);
    sqlite3_clear_bindings(m_stmt);
  }

  int columnInt(int col) const { return sqlite3_column_int(m_stmt, col); }

  int64_t columnInt64(int col) const { return sqlite3_column_int64(m_stmt, col); }

  uint64_t columnUInt64(int col) const { return static_cast<uint64_t>(sqlite3_column_int64(m_stmt, col)); }

  double columnDouble(int col) const { return sqlite3_column_double(m_stmt, col); }

  std::string columnText(int col) const {
    auto *text = reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, col));
    if (!text) return {};
    return {text, static_cast<size_t>(sqlite3_column_bytes(m_stmt, col))};
  }

  QString columnQString(int col) const {
    auto *text = reinterpret_cast<const char *>(sqlite3_column_text(m_stmt, col));
    if (!text) return {};
    return QString::fromUtf8(text, sqlite3_column_bytes(m_stmt, col));
  }

  bool isNull(int col) const { return sqlite3_column_type(m_stmt, col) == SQLITE_NULL; }

  std::string lastError() const { return sqlite3_errmsg(m_db); }
};

class Transaction {
  sqlite3 *m_db = nullptr;
  bool m_done = false;

public:
  explicit Transaction(sqlite3 *db) : m_db(db) { sqlite3_exec(m_db, "BEGIN", nullptr, nullptr, nullptr); }

  ~Transaction() {
    if (!m_done && m_db) sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
  }

  Transaction(const Transaction &) = delete;
  Transaction &operator=(const Transaction &) = delete;

  Transaction(Transaction &&other) noexcept : m_db(other.m_db), m_done(other.m_done) {
    other.m_db = nullptr;
    other.m_done = true;
  }

  Transaction &operator=(Transaction &&other) noexcept {
    if (this != &other) {
      if (!m_done && m_db) sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
      m_db = other.m_db;
      m_done = other.m_done;
      other.m_db = nullptr;
      other.m_done = true;
    }
    return *this;
  }

  bool commit() {
    m_done = true;
    return sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr) == SQLITE_OK;
  }

  void rollback() {
    m_done = true;
    sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
  }
};

class Database {
  sqlite3 *m_handle = nullptr;

  explicit Database(sqlite3 *handle) : m_handle(handle) {}

public:
  Database() = default;

  ~Database() {
    if (m_handle) sqlite3_close_v2(m_handle);
  }

  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  Database(Database &&other) noexcept : m_handle(other.m_handle) { other.m_handle = nullptr; }

  Database &operator=(Database &&other) noexcept {
    if (this != &other) {
      if (m_handle) sqlite3_close_v2(m_handle);
      m_handle = other.m_handle;
      other.m_handle = nullptr;
    }
    return *this;
  }

  static std::expected<Database, std::string> open(const std::filesystem::path &path) {
    sqlite3 *handle = nullptr;
    int const rc = sqlite3_open_v2(
        path.c_str(), &handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (rc != SQLITE_OK) {
      std::string err = handle ? sqlite3_errmsg(handle) : "failed to allocate sqlite handle";
      if (handle) sqlite3_close_v2(handle);
      return std::unexpected(std::move(err));
    }
    return Database(handle);
  }

  Statement prepare(std::string_view sql) const {
    sqlite3_stmt *stmt = nullptr;
    int const rc = sqlite3_prepare_v2(m_handle, sql.data(), static_cast<int>(sql.size()), &stmt, nullptr);
    if (rc != SQLITE_OK) {
      qWarning("db::prepare failed: %s (sql: %.*s)", sqlite3_errmsg(m_handle), static_cast<int>(sql.size()),
               sql.data());
      return {};
    }
    return {stmt, m_handle};
  }

  bool exec(const char *sql) const {
    return sqlite3_exec(m_handle, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
  }

  bool exec(const std::string &sql) const { return exec(sql.c_str()); }

  Transaction transaction() const { return Transaction(m_handle); }

  int64_t lastInsertRowId() const { return sqlite3_last_insert_rowid(m_handle); }

  int changes() const { return sqlite3_changes(m_handle); }

  std::string lastError() const { return sqlite3_errmsg(m_handle); }

  sqlite3 *handle() { return m_handle; }
};

} // namespace db
