#include "program-db/program-db.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
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

std::optional<fs::path> ProgramDb::programPath(std::string_view name) {
  auto isRegularFile = [](const fs::path &path) { return fs::is_regular_file(path); };

  if (isRegularFile(name)) return name;

  auto candidates =
      Omnicast::systemPaths() | std::views::transform([&](const fs::path &path) { return path / name; });
  std::error_code const ec;

  if (auto it = std::ranges::find_if(candidates, isRegularFile); it != candidates.end()) { return *it; }

  return std::nullopt;
}

std::vector<Scored<fs::path>> ProgramDb::search(std::string_view query, int limit) const {
  std::vector<Scored<fs::path>> filtered;

  filtered.reserve(m_progs.size());

  fuzzy::Query const fuzzyQuery{query};
  for (const auto &prog : m_progs) {
    auto const m = fuzzy::scoreWeighted({{prog.string(), 1.0}}, fuzzyQuery);
    if (m.accepted() || query.empty()) { filtered.push_back({prog, m.score}); }
  }

  std::ranges::stable_sort(filtered, std::greater{});

  return filtered;
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
