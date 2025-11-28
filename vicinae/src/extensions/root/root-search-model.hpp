#pragma once
#include "services/app-service/app-service.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/vlist/vlist.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/vlist/common/section-model.hpp"
#include "services/files-service/file-service.hpp"

// clang-format off
enum class SectionType {
	Link, Calculator, Results, Files, Fallback,
	Favorites
};
// clang-format on

struct FallbackItem {
  const RootItem *item;
};

struct FavoriteItem {
  const RootItem *item;
};

struct LinkItem {
  std::shared_ptr<AbstractApplication> app;
  QString url;
};

using RootItemVariant = std::variant<AbstractCalculatorBackend::CalculatorResult, const RootItem *,
                                     std::filesystem::path, FallbackItem, FavoriteItem, LinkItem>;

class RootSearchModel : public vicinae::ui::SectionListModel<RootItemVariant, SectionType> {
  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;

public:
  RootSearchModel(RootItemManager *manager, FileService *fs, AppService *appDb,
                  CalculatorService *calculator);

  bool shouldFastTrack(const RootItemVariant &item) const {
    const auto handleRootItem = [&](const RootItem *item) {
      if (!item->supportsAliasSpaceShortcut()) return false;

      auto alias = m_manager->itemMetadata(item->uniqueId()).alias;
      return !alias.empty() && alias.starts_with(query);
    };

    const auto visitor = overloads{[&](const RootItem *item) { return handleRootItem(item); },
                                   [&](const FavoriteItem &item) { return handleRootItem(item.item); },
                                   [&](auto &&a) { return false; }};

    return std::visit(visitor, item);
  }

  void reloadSearch() { setFilter(query); }

  void regenerateFallback() { m_fallbackItems = m_manager->fallbackItems(); }
  void regenerateFavorites() { m_favorites = m_manager->queryFavorites(); }

  std::span<const SectionType> sections() const {
    if (query.empty()) return m_rootSections;
    return m_searchSections;
  }

  /**
   * Whether root search should also search for files for queries longer
   * than three characters.
   */
  void setFileSearch(bool value);

  void setFilter(std::string_view text);

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
  static constexpr const int MIN_FS_TEXT_LENGTH = 3;

  static constexpr const std::array<SectionType, 2> m_rootSections = {SectionType::Favorites,
                                                                      SectionType::Results};
  static constexpr const std::array<SectionType, 5> m_searchSections = {
      SectionType::Link, SectionType::Calculator, SectionType::Results, SectionType::Files,
      SectionType::Fallback};

  // fired on calculator debounce timeout
  void startCalculator();
  void handleCalculatorFinished();
  void startFileSearch();
  void handleFileSearchFinished();

  std::string query;
  std::span<RootItemManager::ScoredItem> m_items;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calc;

  QFutureWatcher<AbstractCalculatorBackend::ComputeResult> m_calcWatcher; // async calculator invocation
  FileSearchWatcher m_fileWatcher;                                        // async file search computation

  std::vector<IndexerFileResult> m_files;
  std::vector<std::shared_ptr<RootItem>> m_fallbackItems;

  // without query only
  std::vector<RootItemManager::SearchableRootItem> m_favorites;

  std::optional<LinkItem> m_defaultOpener;

  std::string m_fallbackSectionTitle;
  std::string m_resultSectionTitle;

  bool m_isFileSearchEnabled = false;

  // DI
  RootItemManager *m_manager = nullptr;
  FileService *m_fs = nullptr;
  AppService *m_appDb = nullptr;
  CalculatorService *m_calculator = nullptr;

  QTimer m_fileSearchDebounce;
  QTimer m_calculatorDebounce;

  std::string m_fileSearchQuery;
  std::string m_calculatorSearchQuery;
};
