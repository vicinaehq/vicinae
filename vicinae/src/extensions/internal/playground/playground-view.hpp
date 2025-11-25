#include "common.hpp"
#include "layout.hpp"
#include "lib/fts_fuzzy.hpp"
#include "services/emoji-service/emoji.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/list-section-header.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist/vlist.hpp"
#include <qwizard.h>
#include <ranges>

/**
 * A simple model that render regular list items in an horizontal list view, divided into sections.
 */
class HorizontalListModel : public vicinae::ui::VListModel {
public:
  struct Item {
    std::string name;
    std::optional<std::string> subtitle;
    std::optional<ImageURL> icon;
  };

  struct Section {
    std::string name;
    std::vector<Item> items;
  };

  struct ScoredItem {
    const Item *item = nullptr;
    int score = 0;
  };

  struct FilteredSection {
    const Section *section;
    std::vector<ScoredItem> items;
    int bestScore = 0;
  };

  HorizontalListModel() {}

  void clear() {
    m_sections.clear();
    m_filteredSections.clear();
  }

  void addSection(std::string_view name, const std::vector<Item> &&items) {
    m_sections.emplace_back(Section{.name = std::string(name), .items = items});
  }

  void update(const std::function<void(void)> &fn) {
    fn();
    finishUpdate();
  }

  void finishUpdate() { setFilter(""); }

  void setPreserveSectionOrder(bool value) { m_preserveSectionOrder = value; }

  void setFilter(std::string_view text) {
    m_filteredSections.clear();
    for (const auto &section : m_sections) {
      FilteredSection sec;
      sec.section = &section;
      sec.items.reserve(section.items.size());

      auto scored = section.items | std::views::transform([&](const Item &item) {
                      int score = 0;
                      fts::fuzzy_match(text, item.name, score);
                      return ScoredItem{.item = &item, .score = score};
                    }) |
                    std::views::filter([&](auto &&item) { return text.empty() || item.score > 0; });

      for (const auto &&item : scored) {
        sec.bestScore = std::max(sec.bestScore, item.score);
        sec.items.emplace_back(item);
      }

      std::ranges::stable_sort(sec.items, [](auto a, auto b) { return a.score > b.score; });

      if (!sec.items.empty()) { m_filteredSections.emplace_back(sec); }
    }

    if (!m_preserveSectionOrder) {
      std::ranges::sort(m_filteredSections, [](auto &&a, auto &&b) { return a.bestScore > b.bestScore; });
    }

    emit dataChanged();
  }

  bool isSectionHeading(Index idx) const {
    return std::holds_alternative<const Section *>(*fromFlatIdx(idx));
  }

  bool isSelectable(Index idx) const override { return !isSectionHeading(idx); }

  size_t heightAtIndex(Index idx) const override {
    size_t height = 0;
    size_t i = 0;

    for (const auto &section : m_filteredSections) {
      if (i == idx) return height;
      height += HEADER_HEIGHT;
      ++i;

      for (const auto &item : section.items) {
        if (i == idx) return height;
        height += ITEM_HEIGHT;
        ++i;
      }
    }

    return height;
  }

  Index indexAtHeight(int target) const override {
    size_t height = 0;
    size_t i = 0;

    for (const auto &section : m_filteredSections) {
      if (target >= height && target < height + HEADER_HEIGHT) return i;
      height += HEADER_HEIGHT;
      ++i;
      for (const auto &item : section.items) {
        if (target >= height && target < height + ITEM_HEIGHT) return i;
        height += ITEM_HEIGHT;
        ++i;
      }
    }

    return -1;
  }

  std::optional<std::variant<const Section *, const Item *>> fromFlatIdx(Index idx) const {
    int i = 0;
    for (const auto &section : m_filteredSections) {
      if (i == idx) return section.section;
      ++i;
      if (idx >= i && idx < i + section.items.size()) { return section.items.at(idx - i).item; }
      i += section.items.size();
    }

    return std::nullopt;
  }

  StableID stableId(size_t idx) const override {
    auto item = fromFlatIdx(idx);
    const auto visitor = overloads{[&](const Item *item) { return m_hasher(item->name); },
                                   [&](const Section *str) { return m_hasher(str->name); }};
    return std::visit(visitor, item.value());
  }

  size_t height() const override {
    return std::ranges::fold_left(m_filteredSections, 0, [](int height, const FilteredSection &sec) {
      return height + HEADER_HEIGHT + ITEM_HEIGHT * sec.items.size();
    });
  }

  size_t height(size_t idx) const override { return ITEM_HEIGHT; }

  size_t count() const override {
    size_t count = 0;
    for (const auto &section : m_filteredSections) {
      count = count + section.items.size() + 1; // +1 for section heading
    }
    return count;
  };

  WidgetTag widgetTag(Index idx) const override {
    if (isSectionHeading(idx)) return 0;
    return 1;
  }

  WidgetType *createWidget(size_t idx) const override {
    if (isSectionHeading(idx)) { return new OmniListSectionHeader("", "", 0); }
    return new DefaultListItemWidget;
  }

  void refreshWidget(size_t idx, WidgetType *widget) const override {
    auto item = fromFlatIdx(idx);
    const auto visitor = overloads{[&](const Item *item) {
                                     auto w = static_cast<DefaultListItemWidget *>(widget);
                                     w->setName(item->name.c_str());
                                     w->setIconUrl(item->icon);
                                   },
                                   [&](const Section *str) {
                                     auto w = static_cast<OmniListSectionHeader *>(widget);
                                     w->setTitle(str->name.c_str());
                                   }};
    std::visit(visitor, *item);
  }

private:
  bool m_preserveSectionOrder = false;

  std::vector<FilteredSection> m_filteredSections;
  std::vector<Section> m_sections;
  std::hash<std::string_view> m_hasher;

  static constexpr const int ITEM_HEIGHT = 40;
  static constexpr const int HEADER_HEIGHT = 40;
};

class PlaygroundView : public BaseView {
public:
  PlaygroundView() {
    VStack().add(m_list).imbue(this);
    m_list->setModel(m_model);

    std::vector<HorizontalListModel::Item> items;

    for (const auto &emoji : StaticEmojiDatabase::orderedList()) {
      items.emplace_back(HorizontalListModel::Item{
          .name = std::string(emoji.name),
          .icon = ImageURL::emoji(QString::fromUtf8(emoji.emoji.data(), emoji.emoji.size()))});
    }

    m_model->update([&]() { m_model->addSection("Emojis", std::move(items)); });
    m_list->calculate();
    m_list->selectFirst();
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Up:
      return m_list->selectUp();
    case Qt::Key_Down:
      return m_list->selectDown();
    }
    return false;
  }

  void textChanged(const QString &text) override {
    m_model->setFilter(text.toStdString());
    m_list->selectFirst();
  }

private:
  HorizontalListModel *m_model = new HorizontalListModel();
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
