#pragma once
#include <iostream>
#include <mutex>
#include <sstream>
#include <string_view>

// Thread-safe stderr logger; each line carries a level token the parent's [F] forwarder parses.
namespace flog {

enum class Level { Debug, Info, Warn, Error };

inline std::mutex &mutex() {
  static std::mutex m;
  return m;
}

inline std::string_view token(Level level) {
  switch (level) {
  case Level::Debug:
    return "debug";
  case Level::Info:
    return "info";
  case Level::Warn:
    return "warn";
  case Level::Error:
    return "error";
  }
  return "info";
}

class LogStream {
  std::ostringstream m_oss;
  Level m_level;
  bool m_first = true;

public:
  explicit LogStream(Level level) : m_level(level) {}

  LogStream &noquote() { return *this; }

  template <typename T> LogStream &operator<<(const T &value) {
    if (!m_first) m_oss << ' ';
    m_first = false;
    m_oss << value;
    return *this;
  }

  ~LogStream() {
    std::scoped_lock const lock(mutex());
    std::cerr << token(m_level) << '\t' << m_oss.str() << '\n';
  }
};

inline LogStream debug() { return LogStream(Level::Debug); }
inline LogStream info() { return LogStream(Level::Info); }
inline LogStream warn() { return LogStream(Level::Warn); }
inline LogStream error() { return LogStream(Level::Error); }

}; // namespace flog
