#include "root-search-model.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/transform-result/transform-result.hpp"
#include <format>

RootSearchModel::RootSearchModel(RootItemManager *manager) : m_manager(manager) {}

void RootSearchModel::setSearchResults(const SearchResults &results) {
  query = results.query;
  m_items = results.items;
  m_calc = results.calculator;
  m_files = results.files;
  m_defaultOpener = results.defaultOpener;
  m_resultSectionTitle = std::format("Results ({})", m_items.size());
  m_fallbackSectionTitle = std::format("Use \"{}\" with...", query);

  emit dataChanged();
}

void RootSearchModel::setQuery(std::string_view text) {
  query = text;
  m_resultSectionTitle = std::format("Results ({})", m_items.size());
  m_fallbackSectionTitle = std::format("Use \"{}\" with...", query);
  emit dataChanged();
}

void RootSearchModel::setItems(std::vector<RootItemManager::ScoredItem> items) {
  m_items = items;
  m_resultSectionTitle = std::format("Results ({})", m_items.size());
  emit dataChanged();
}

void RootSearchModel::setCalculatorResult(const AbstractCalculatorBackend::CalculatorResult &result) {
  m_calc = result;
  emit dataChanged();
}

void RootSearchModel::setFileResults(const std::vector<IndexerFileResult> &files) {
  m_files = files;
  emit dataChanged();
}

void RootSearchModel::setFallbackItems(const std::vector<std::shared_ptr<RootItem>> &items) {
  m_fallbackItems = items;
}

void RootSearchModel::setFavorites(const std::vector<std::shared_ptr<RootItem>> &favorites) {
  m_favorites = favorites;
}

void RootSearchModel::setDefaultOpener(const LinkItem &opener) {
  m_defaultOpener = opener;
  emit dataChanged();
}

int RootSearchModel::sectionCount() const { return sections().size(); }

int RootSearchModel::sectionItemHeight(SectionType id) const {
  switch (id) {
  case SectionType::Calculator:
    return CALCULATOR_HEIGHT;
  default:
    return ITEM_HEIGHT;
  }
}

SectionType RootSearchModel::sectionIdFromIndex(int idx) const { return sections()[idx]; }

int RootSearchModel::sectionItemCount(SectionType id) const {
  switch (id) {
  case SectionType::Link:
    return m_defaultOpener.has_value();
  case SectionType::Calculator:
    return m_calc.has_value();
  case SectionType::Results:
    return m_items.size();
  case SectionType::Files:
    return m_files.size();
  case SectionType::Fallback:
    return m_fallbackItems.size();
  case SectionType::Favorites:
    return m_favorites.size();
  default:
    return 0;
  }
}

std::string_view RootSearchModel::sectionName(SectionType id) const {
  switch (id) {
  case SectionType::Link:
    return "Link";
  case SectionType::Calculator:
    return "Calculator";
  case SectionType::Results:
    return query.empty() ? std::string_view("Suggestions") : m_resultSectionTitle;
  case SectionType::Files:
    return "Files";
  case SectionType::Fallback:
    return m_fallbackSectionTitle;
  case SectionType::Favorites:
    return "Favorites";
  default:
    return "Unknown";
  }
}

RootItemVariant RootSearchModel::sectionItemAt(SectionType id, int itemIdx) const {
  switch (id) {
  case SectionType::Link:
    return m_defaultOpener.value();
  case SectionType::Calculator:
    return m_calc.value();
  case SectionType::Results:
    return RootSearchResult{.scored = &m_items.at(itemIdx)};
  case SectionType::Files:
    return m_files[itemIdx].path;
  case SectionType::Fallback:
    return FallbackItem{.item = m_fallbackItems.at(itemIdx).get()};
  case SectionType::Favorites:
    return FavoriteItem{.item = m_favorites.at(itemIdx).get()};
  }

  return {};
}

RootSearchModel::StableID RootSearchModel::stableId(const RootItemVariant &item) const {
  static std::hash<std::string> hasher = {};

  const auto visitor = overloads{
      [&](const AbstractCalculatorBackend::CalculatorResult &) { return randomId(); },
      [](const RootSearchResult &item) {
        return std::hash<EntrypointId>()(item.scored->item.get()->uniqueId());
      },
      [](const LinkItem &item) { return hasher(item.url.toStdString() + ".url"); },
      [](const std::filesystem::path &path) { return hasher(path.string() + ".files"); },
      [](const FallbackItem &item) {
        return hasher(std::string{item.item->uniqueId()} + std::string_view{".fallback"});
      },
      [](const FavoriteItem &item) { return hasher(std::string{item.item->uniqueId()} + ".favorite"); }};

  return std::visit(visitor, item);
}

RootSearchModel::WidgetTag RootSearchModel::widgetTag(const RootItemVariant &item) const {
  if (std::holds_alternative<AbstractCalculatorBackend::CalculatorResult>(item)) { return InvalidTag; }
  return item.index();
}

RootSearchModel::WidgetType *RootSearchModel::createItemWidget(const RootItemVariant &type) const {
  const auto visitor = overloads{
      [](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * { return new TransformResult; },
      [](const RootSearchResult &) -> WidgetType * { return new DefaultListItemWidget; },
      [](const std::filesystem::path &path) -> WidgetType * { return new DefaultListItemWidget; },
      [](const LinkItem &item) -> WidgetType * { return new DefaultListItemWidget; },
      [](const FallbackItem &item) -> WidgetType * { return new DefaultListItemWidget; },
      [](const FavoriteItem &item) -> WidgetType * { return new DefaultListItemWidget; }};

  return std::visit(visitor, type);
}

void RootSearchModel::refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const {
  auto refreshRootItem = [&](const RootItem *item, bool showAlias = true) {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    w->setAlias(showAlias ? m_manager->itemMetadata(item->uniqueId()).alias.value_or("").c_str() : "");
    w->setName(item->displayName());
    w->setIconUrl(item->iconUrl());
    w->setSubtitle(item->subtitle());
    w->setAccessories(item->accessories());
    w->setActive(item->isActive());
  };
  const auto visitor =
      overloads{[&](const AbstractCalculatorBackend::CalculatorResult &calc) {
                  auto w = static_cast<TransformResult *>(widget);
                  const auto toDp = [](auto &&u) { return u.displayName; };
                  w->setBase(calc.question.text, calc.question.unit.transform(toDp).value_or("Expression"));
                  w->setResult(calc.answer.text, calc.answer.unit.transform(toDp).value_or("Answer"));
                },
                [&](const std::filesystem::path &path) {
                  auto w = static_cast<DefaultListItemWidget *>(widget);
                  w->setName(getLastPathComponent(path).c_str());
                  w->setIconUrl(ImageURL::fileIcon(path));
                  w->setSubtitle(compressPath(path));
                  w->setActive(false);
                },
                [&](const LinkItem &item) {
                  auto w = static_cast<DefaultListItemWidget *>(widget);
                  w->setName(item.url);
                  w->setIconUrl(item.app->iconUrl());
                  w->setActive(false);
                },
                [&](const RootSearchResult &res) { refreshRootItem(res.scored->item.get().get(), true); },
                [&](const FallbackItem &item) { refreshRootItem(item.item, false); },
                [&](const FavoriteItem &item) { refreshRootItem(item.item); }};

  std::visit(visitor, type);
}
