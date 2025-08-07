#pragma once
#include "services/root-item-manager/root-item-manager.hpp"
#include <qstringview.h>

/**
 * To search root items from a query.
 * This only performs string based fuzzy search, the results are sorted
 * according to frecency rules later on outside this class.
 */
class RootSearcher {
  double computeExactStringScore(QStringView str, QStringView query);
  double computeExactScore(const RootItem &item, QStringView query);
  double computeFuzzyScore(const RootItem &item, QStringView query);
  double clampScore(double v);

public:
  struct ScoredItem {
    double score;
    std::shared_ptr<RootItem> item;
  };

  std::vector<ScoredItem> search(const std::vector<std::shared_ptr<RootItem>> &items, QStringView s);
};
