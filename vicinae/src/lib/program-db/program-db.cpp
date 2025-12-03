#include "program-db/program-db.hpp"
#include "fuzzy/weighted-fuzzy-scorer.hpp"
#include "utils.hpp"
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
  std::error_code ec;

  if (auto it = std::ranges::find_if(candidates, isRegularFile); it != candidates.end()) { return *it; }

  return std::nullopt;
}

std::vector<Scored<fs::path>> ProgramDb::search(std::string_view query, int limit) const {
  std::vector<Scored<fs::path>> filtered;
  fuzzy::FuzzyScorer<fs::path> scorer;

  scorer.score(
      m_progs, query,
      [](std::string_view query, const fs::path &path) {
        int score = 0;
        fts::fuzzy_match(query, path.c_str(), score);
        return score;
      },
      filtered);

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
