#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <cstdint>

enum class SectionType : std::uint8_t { Link, Calculator, Results, Files, Fallback, Favorites };

struct FallbackItem {
  const RootItem *item;
};

struct FavoriteItem {
  const RootItem *item;
};

struct RootSearchResult {
  const RootItemManager::ScoredItem *scored;
};

struct LinkItem {
  std::shared_ptr<AbstractApplication> app;
  QString url;
};

using RootItemVariant = std::variant<AbstractCalculatorBackend::CalculatorResult, RootSearchResult,
                                     std::filesystem::path, FallbackItem, FavoriteItem, LinkItem>;

struct SearchResults {
  std::string query;
  std::vector<RootItemManager::ScoredItem> items;
  std::optional<AbstractCalculatorBackend::CalculatorResult> calculator;
  std::vector<IndexerFileResult> files;
  std::optional<LinkItem> defaultOpener;
};
