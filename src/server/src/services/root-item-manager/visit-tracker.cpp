#include "services/root-item-manager/visit-tracker.hpp"
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/key_transformers.hpp>
#include <qlogging.h>
#include <QDateTime>
#include <algorithm>

namespace fs = std::filesystem;

VisitTracker::VisitTracker(const fs::path &path) : m_path(path) { loadFromDisk(); }

void VisitTracker::registerVisit(const EntrypointId &id, std::string_view query) {
  VisitInfo &data = m_data.visited[id];
  data.lastVisitedAt = QDateTime::currentSecsSinceEpoch();
  ++data.visitCount;
  if (!query.empty()) {
    auto now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());
    auto &event = m_data.usageEvents.emplace_back(Data::UsageEvent{.itemId = std::string{id},
                                                                  .query = std::string{query},
                                                                  .visitedAt = now});
    (void)event;
  }

  pruneUsageEvents();
  rebuildRollingUsageIndex();

  // todo: we might want to flush ever X seconds instead of saving directly
  saveToDisk();
}

VisitTracker::VisitInfo VisitTracker::getVisit(const EntrypointId &id) const {
  if (auto it = m_data.visited.find(id); it != m_data.visited.end()) { return it->second; }
  return {};
}

VisitTracker::QueryUsageInfo VisitTracker::getRollingUsage(const EntrypointId &id) const {
  if (auto it = m_rollingUsageByItem.find(std::string{id}); it != m_rollingUsageByItem.end()) {
    return it->second;
  }
  return {};
}

VisitTracker::QueryUsageInfo VisitTracker::getQueryUsage(std::string_view query, const EntrypointId &id) const {
  if (query.empty()) return {};
  if (auto it = m_queryUsageByText.find(std::string{query}); it != m_queryUsageByText.end()) {
    if (auto it2 = it->second.find(std::string{id}); it2 != it->second.end()) { return it2->second; }
  }
  return {};
}

void VisitTracker::forget(const EntrypointId &id) {
  m_data.visited.erase(id);
  std::string const sid = id;
  std::erase_if(m_data.usageEvents, [&](const auto &event) { return event.itemId == sid; });
  rebuildRollingUsageIndex();
  saveToDisk();
}

void VisitTracker::loadFromDisk() {
  if (auto error = glz::read_file_jsonc(m_data, m_path.c_str(), m_buf)) {
    qWarning() << "Failed to load visits file from" << m_path.c_str() << glz::format_error(error);
  }

  pruneUsageEvents();
  rebuildRollingUsageIndex();
}

void VisitTracker::saveToDisk() {
  if (auto error = glz::write_file_json(m_data, m_path.c_str(), m_buf)) {
    qWarning() << "Failed to save visit to" << m_path.c_str() << glz::format_error(error);
  }
}

void VisitTracker::pruneUsageEvents() {
  auto const now = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());
  auto const cutoff = now > ROLLING_WINDOW_SECONDS ? now - ROLLING_WINDOW_SECONDS : 0;

  auto firstFresh = std::ranges::find_if(m_data.usageEvents, [&](const auto &event) {
    return event.visitedAt >= cutoff;
  });

  if (firstFresh != m_data.usageEvents.begin()) {
    m_data.usageEvents.erase(m_data.usageEvents.begin(), firstFresh);
  }
}

void VisitTracker::rebuildRollingUsageIndex() {
  m_rollingUsageByItem.clear();
  m_queryUsageByText.clear();

  for (const auto &event : m_data.usageEvents) {
    auto &itemUsage = m_rollingUsageByItem[event.itemId];
    itemUsage.lastVisitedAt = event.visitedAt;
    ++itemUsage.visitCount;

    auto &queryUsage = m_queryUsageByText[event.query][event.itemId];
    queryUsage.lastVisitedAt = event.visitedAt;
    ++queryUsage.visitCount;
  }
}

template <> struct glz::meta<VisitTracker::VisitInfo> : glz::snake_case {};
template <> struct glz::meta<VisitTracker::Data> : glz::snake_case {};
template <> struct glz::meta<VisitTracker::QueryUsageInfo> : glz::snake_case {};
template <> struct glz::meta<VisitTracker::Data::UsageEvent> : glz::snake_case {};
