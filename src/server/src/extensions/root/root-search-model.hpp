#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/vlist/vlist.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "vicinae-ipc/ipc.hpp"
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

class RootSearchModel : public vicinae::ui::SectionListModel<RootItemVariant, SectionType> {
public:
  RootSearchModel(RootItemManager *manager);

  bool shouldFastTrack(const RootItemVariant &item) const {
    const auto handleRootItem = [&](const RootItem *item) {
      if (!item->supportsAliasSpaceShortcut()) return false;
      auto alias = m_manager->itemMetadata(item->uniqueId()).alias;
      return alias && alias->starts_with(query);
    };

    const auto visitor = overloads{
        [&](const RootSearchResult &item) { return handleRootItem(item.scored->item.get().get()); },
        [&](const FavoriteItem &item) { return handleRootItem(item.item); }, [&](auto &&a) { return false; }};

    return std::visit(visitor, item);
  }

  std::span<const SectionType> sections() const {
    if (query.empty()) return m_rootSections;
    return m_searchSections;
  }

  void reset();
  void setSearchResults(const SearchResults &results);
  void setQuery(std::string_view text);
  void setItems(std::vector<RootItemManager::ScoredItem> items);
  void setCalculatorResult(const AbstractCalculatorBackend::CalculatorResult &result);
  void setFileResults(const std::vector<IndexerFileResult> &files);
  void setFallbackItems(const std::vector<std::shared_ptr<RootItem>> &items);
  void setFavorites(const std::vector<std::shared_ptr<RootItem>> &favorites);
  void setDefaultOpener(const LinkItem &opener);

protected:
  int sectionCount() const override;
  int sectionItemHeight(SectionType id) const override;
  SectionType sectionIdFromIndex(int idx) const override;

  int sectionItemCount(SectionType id) const override;

  std::string_view sectionName(SectionType id) const override;

  RootItemVariant sectionItemAt(SectionType id, int itemIdx) const override;

  StableID stableId(const RootItemVariant &item) const override;

  WidgetTag widgetTag(const RootItemVariant &item) const override;

  WidgetType *createItemWidget(const RootItemVariant &type) const override;

  void refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const override;

private:
  static constexpr const size_t ITEM_HEIGHT = 41;
  static constexpr const size_t CALCULATOR_HEIGHT = 90;

  static constexpr const auto m_rootSections = std::array{SectionType::Favorites, SectionType::Results};
  static constexpr const auto m_searchSections =
      std::array{SectionType::Link, SectionType::Calculator, SectionType::Results, SectionType::Files,
                 SectionType::Fallback};

  std::string query;
  std::vector<RootItemManager::ScoredItem> m_items;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calc;
  std::vector<IndexerFileResult> m_files;
  std::vector<std::shared_ptr<RootItem>> m_fallbackItems;
  std::vector<std::shared_ptr<RootItem>> m_favorites;
  std::optional<LinkItem> m_defaultOpener;

  std::string m_fallbackSectionTitle;
  std::string m_resultSectionTitle;

  RootItemManager *m_manager = nullptr;
};
