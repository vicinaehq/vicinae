#pragma once
#include <QByteArray>
#include <QElapsedTimer>
#include <QString>
#include <mutex>
#include <unordered_map>
#include <QHash>

namespace image_data_cache_detail {
struct QStringHash {
  size_t operator()(const QString &s) const { return qHash(s); }
};
} // namespace image_data_cache_detail

class ImageDataCache {
public:
  static ImageDataCache &instance() {
    static ImageDataCache s;
    return s;
  }

  void storeAnimated(const QString &key, const QByteArray &data) {
    std::lock_guard lock(m_mutex);
    evictStale();
    if (m_entries.size() >= kMaxEntries) evictOldest();
    m_entries[key] = Entry{data, currentMs(), false};
  }

  void storeNotAnimated(const QString &key) {
    std::lock_guard lock(m_mutex);
    evictStale();
    if (m_entries.size() >= kMaxEntries) evictOldest();
    m_entries[key] = Entry{{}, currentMs(), true};
  }

  enum class Result { Miss, NotAnimated, Animated };

  struct TakeResult {
    Result result;
    QByteArray data;
  };

  TakeResult take(const QString &key) {
    std::lock_guard lock(m_mutex);
    auto it = m_entries.find(key);
    if (it == m_entries.end()) return {Result::Miss, {}};
    if (currentMs() - it->second.timestamp > kTtlMs) {
      m_entries.erase(it);
      return {Result::Miss, {}};
    }
    bool tombstone = it->second.tombstone;
    auto data = std::move(it->second.data);
    m_entries.erase(it);
    return {tombstone ? Result::NotAnimated : Result::Animated, std::move(data)};
  }

private:
  static constexpr int kMaxEntries = 32;
  static constexpr qint64 kTtlMs = 30000;

  struct Entry {
    QByteArray data;
    qint64 timestamp;
    bool tombstone;
  };

  static qint64 currentMs() {
    static QElapsedTimer timer = []() {
      QElapsedTimer t;
      t.start();
      return t;
    }();
    return timer.elapsed();
  }

  void evictStale() {
    qint64 now = currentMs();
    for (auto it = m_entries.begin(); it != m_entries.end();) {
      if (now - it->second.timestamp > kTtlMs)
        it = m_entries.erase(it);
      else
        ++it;
    }
  }

  void evictOldest() {
    auto oldest = m_entries.begin();
    for (auto it = m_entries.begin(); it != m_entries.end(); ++it)
      if (it->second.timestamp < oldest->second.timestamp) oldest = it;
    if (oldest != m_entries.end()) m_entries.erase(oldest);
  }

  std::mutex m_mutex;
  std::unordered_map<QString, Entry, image_data_cache_detail::QStringHash> m_entries;
};
