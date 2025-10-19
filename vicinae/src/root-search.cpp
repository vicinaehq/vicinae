#include "root-search.hpp"
#include "rapidfuzz/fuzz.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <qlogging.h>
#include <qnamespace.h>

static const double NAME_WEIGHT = 0.8;
static const double SUBTITLE_WEIGHT = 0.3;

double RootSearcher::computeExactStringScore(QStringView str, QStringView query) {
  if (str == query) { return 1; };
  if (str.startsWith(query, Qt::CaseInsensitive)) { return 0.9; };

  double score = 0;
  // TODO: we could use more advanced tokenization in the future, for now it's fine
  auto words = str.split(' ');

  if (words.empty()) return 0;

  size_t matchCount = 0;

  for (const auto &word : words) {
    matchCount += word.startsWith(query, Qt::CaseInsensitive);
  }

  return static_cast<double>(matchCount) / words.size();
}

double RootSearcher::clampScore(double v) { return std::clamp(v, 0.0, 1.0); }

RootItemMetadata RootSearcher::metadata(const QString &id) const {
  if (auto it = m_meta.find(id); it != m_meta.end()) { return it->second; }

  return {};
}

double RootSearcher::computeExactScore(const RootItem &item, QStringView query) {
  auto meta = metadata(item.uniqueId());
  double nameScore = computeExactStringScore(item.displayName(), query) * NAME_WEIGHT;
  double aliasScore = computeExactStringScore(meta.alias, query) * NAME_WEIGHT;
  double subtitleScore = computeExactStringScore(item.subtitle(), query) * SUBTITLE_WEIGHT;
  double keywordScore = 0;

  for (const auto &kw : item.keywords()) {
    keywordScore += computeExactStringScore(kw, query);
  }

  keywordScore *= 0.3;

  return clampScore(nameScore + subtitleScore + keywordScore + aliasScore);
}

double RootSearcher::computeFuzzyScore(const RootItem &item, QStringView query) {
  namespace fuzz = rapidfuzz::fuzz;
  double nameScore =
      fuzz::partial_ratio(item.displayName().toStdString(), query.toString().toStdString(), 70);

  return clampScore(nameScore / 100);
}

std::vector<RootSearcher::ScoredItem>
RootSearcher::search(const std::vector<std::shared_ptr<RootItem>> &items, QStringView s) {
  std::vector<ScoredItem> results;
  std::string str = s.toString().toStdString();

  results.reserve(100);

  double exactWeight = 0.8;
  double fuzzyWeight = 0.2;

  for (const auto &item : items) {
    double exactScore = computeExactScore(*item, s);
    double fuzzyScore = computeFuzzyScore(*item, s);
    double score = (exactScore * exactWeight) + (fuzzyScore * fuzzyWeight);

    if (score > 0) { results.emplace_back(ScoredItem{.score = score, .item = item}); }
  }

  std::ranges::sort(results, [](auto &&a, auto &&b) { return a.score > b.score; });

  return results;
}

RootSearcher::RootSearcher(const std::unordered_map<QString, RootItemMetadata> &meta) : m_meta(meta) {}
