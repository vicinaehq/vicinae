#include "services/root-item-manager/visit-tracker.hpp"
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <glaze/util/key_transformers.hpp>
#include <qlogging.h>
#include <QDateTime>

namespace fs = std::filesystem;

VisitTracker::VisitTracker(const fs::path &path) : m_path(path) { loadFromDisk(); }

void VisitTracker::registerVisit(const EntrypointId &id) {
  VisitInfo &data = m_data.visited[id];
  data.lastVisitedAt = QDateTime::currentSecsSinceEpoch();
  ++data.visitCount;
  // todo: we might want to flush ever X seconds instead of saving directly
  saveToDisk();
}

VisitTracker::VisitInfo VisitTracker::getVisit(const EntrypointId &id) const {
  if (auto it = m_data.visited.find(id); it != m_data.visited.end()) { return it->second; }
  return {};
}

void VisitTracker::forget(const EntrypointId &id) {
  m_data.visited.erase(id);
  saveToDisk();
}

void VisitTracker::loadFromDisk() {
  if (auto error = glz::read_file_jsonc(m_data, m_path.c_str(), m_buf)) {
    qWarning() << "Failed to load visits file from" << m_path.c_str() << glz::format_error(error);
  }
}

void VisitTracker::saveToDisk() {
  if (auto error = glz::write_file_json(m_data, m_path.c_str(), m_buf)) {
    qWarning() << "Failed to save visit to" << m_path.c_str() << glz::format_error(error);
  }
}

template <> struct glz::meta<VisitTracker::VisitInfo> : glz::snake_case {};
