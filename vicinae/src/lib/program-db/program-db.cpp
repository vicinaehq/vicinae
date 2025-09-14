#include "program-db/program-db.hpp"
#include "vicinae.hpp"
#include <filesystem>
#include <qnamespace.h>
#include <QtConcurrent/QtConcurrent>
#include <ranges>

namespace fs = std::filesystem;

ProgramDb::ProgramDb() {
  connect(m_watcher, &Watcher::finished, this, [this]() {
    if (m_watcher->isCanceled()) { return; }

    m_progs = m_watcher->result();
    emit backgroundScanFinished();
  });
}

std::vector<fs::path> ProgramDb::search(const QString &query, int limit) const {
  // simple for now, we will optimize if needed

  auto filter = [&](const fs::path &path) {
    return QString(path.filename().c_str()).contains(query, Qt::CaseInsensitive);
  };

  return m_progs | std::views::filter(filter) | std::views::take(limit) | std::ranges::to<std::vector>();
}

void ProgramDb::scanSync() { m_progs = scan(); }

void ProgramDb::backgroundScan() { m_watcher->setFuture(QtConcurrent::run(&ProgramDb::scan)); }

const std::vector<std::filesystem::path> &ProgramDb::programs() const { return m_progs; }

std::vector<fs::path> ProgramDb::scan() {
  std::vector<fs::path> results;

  results.reserve(1000);

  for (const auto &path : Omnicast::systemPaths()) {
    std::error_code ec;

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      results.emplace_back(entry.path());
    }
  }

  return results;
}
