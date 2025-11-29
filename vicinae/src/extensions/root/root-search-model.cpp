#include "root-search-model.hpp"
#include "ui/transform-result/transform-result.hpp"

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

void RootSearchModel::setItems(std::span<RootItemManager::ScoredItem> items) {
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
  emit dataChanged();
}

void RootSearchModel::setFavorites(const std::vector<RootItemManager::SearchableRootItem> &favorites) {
  m_favorites = favorites;
  emit dataChanged();
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
    return m_items[itemIdx].item.get().get();
  case SectionType::Files:
    return m_files[itemIdx].path;
  case SectionType::Fallback:
    return FallbackItem{.item = m_fallbackItems[itemIdx].get()};
  case SectionType::Favorites:
    return FavoriteItem{.item = m_favorites[itemIdx].item.get()};
  }

  return {};
}

RootSearchModel::StableID RootSearchModel::stableId(const RootItemVariant &item) const {
  static std::hash<QString> hasher = {};
  const auto visitor =
      overloads{[&](const AbstractCalculatorBackend::CalculatorResult &) { return randomId(); },
                [](const RootItem *item) { return hasher(item->uniqueId()); },
                [](const LinkItem &item) { return hasher(item.url + ".url"); },
                [](const std::filesystem::path &path) { return hasher(QString(path.c_str()) + ".files"); },
                [](const FallbackItem &item) { return hasher(item.item->uniqueId() + ".fallback"); },
                [](const FavoriteItem &item) { return hasher(item.item->uniqueId() + ".favorite"); }};
  return std::visit(visitor, item);
}

RootSearchModel::WidgetTag RootSearchModel::widgetTag(const RootItemVariant &item) const {
  if (std::holds_alternative<AbstractCalculatorBackend::CalculatorResult>(item)) { return InvalidTag; }
  return item.index();
}

RootSearchModel::WidgetType *RootSearchModel::createItemWidget(const RootItemVariant &type) const {
  const auto visitor = overloads{
      [](const AbstractCalculatorBackend::CalculatorResult &) -> WidgetType * { return new TransformResult; },
      [](const RootItem *) -> WidgetType * { return new DefaultListItemWidget; },
      [](const std::filesystem::path &path) -> WidgetType * { return new DefaultListItemWidget; },
      [](const LinkItem &item) -> WidgetType * { return new DefaultListItemWidget; },
      [](const FallbackItem &item) -> WidgetType * { return new DefaultListItemWidget; },
      [](const FavoriteItem &item) -> WidgetType * { return new DefaultListItemWidget; }};

  return std::visit(visitor, type);
}

void RootSearchModel::refreshItemWidget(const RootItemVariant &type, WidgetType *widget) const {
  auto refreshRootItem = [&](const RootItem *item, bool showAlias = true) {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    w->setAlias(showAlias ? m_manager->itemMetadata(item->uniqueId()).alias.c_str() : "");
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
                  w->setSubtitle(path);
                  w->setActive(false);
                },
                [&](const LinkItem &item) {
                  auto w = static_cast<DefaultListItemWidget *>(widget);
                  w->setName(item.url);
                  w->setIconUrl(item.app->iconUrl());
                  w->setActive(false);
                },
                refreshRootItem,
                [&](const FallbackItem &item) { refreshRootItem(item.item, false); },
                [&](const FavoriteItem &item) { refreshRootItem(item.item); }};

  std::visit(visitor, type);
}
