#pragma once
#include "file-indexer/string-util.hpp"
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>

// Thread-safe stderr logger; each line carries a level token the parent's [F] forwarder parses.
namespace flog {

enum class Level { Trace, Debug, Info, Warn, Error, Off };

inline std::mutex &mutex() {
  static std::mutex m;
  return m;
}

inline std::string_view token(Level level) {
  switch (level) {
  case Level::Trace:
    return "trace";
  case Level::Debug:
    return "debug";
  case Level::Info:
    return "info";
  case Level::Warn:
    return "warn";
  case Level::Error:
    return "error";
  case Level::Off:
    return "off";
  }
  return "info";
}

inline Level parseLevel(std::string_view value) {
  std::string normalized = file_indexer::asciiLowercase(value);
  value = normalized;

  if (value == "trace") return Level::Trace;
  if (value == "debug") return Level::Debug;
  if (value == "info") return Level::Info;
  if (value == "warn" || value == "warning") return Level::Warn;
  if (value == "error") return Level::Error;
  if (value == "off" || value == "none" || value == "quiet") return Level::Off;
  return Level::Warn;
}

inline Level configuredLevel() {
  static const Level level = [] {
    const char *env = std::getenv("VICINAE_FILE_INDEXER_LOG");
    if (!env) return Level::Warn;
    return parseLevel(env);
  }();

  return level;
}

inline bool enabled(Level level) {
  return static_cast<int>(level) >= static_cast<int>(configuredLevel()) && configuredLevel() != Level::Off;
}

class LogStream {
  std::ostringstream m_oss;
  Level m_level;
  bool m_enabled;
  bool m_first = true;

public:
  explicit LogStream(Level level) : m_level(level), m_enabled(enabled(level)) {}

  LogStream &noquote() { return *this; }

  template <typename T> LogStream &operator<<(const T &value) {
    if (!m_enabled) return *this;
    if (!m_first) m_oss << ' ';
    m_first = false;
    m_oss << value;
    return *this;
  }

  ~LogStream() {
    if (!m_enabled) return;
    std::scoped_lock const lock(mutex());
    std::cerr << token(m_level) << '\t' << m_oss.str() << '\n';
  }
};

inline LogStream trace() { return LogStream(Level::Trace); }
inline LogStream debug() { return LogStream(Level::Debug); }
inline LogStream info() { return LogStream(Level::Info); }
inline LogStream warn() { return LogStream(Level::Warn); }
inline LogStream error() { return LogStream(Level::Error); }

}; // namespace flog
