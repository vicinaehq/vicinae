#include "common.hpp"
#include "layout.hpp"
#include "lib/fts_fuzzy.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/typography/typography.hpp"
#include "ui/views/base-view.hpp"
#include "ui/vlist.hpp"
#include <absl/strings/internal/str_format/extension.h>
#include <format>
#include <ranges>

class HorizontalListModel : public vicinae::ui::VListModel {
  static constexpr const int ITEM_HEIGHT = 40;
  static constexpr const int HEADER_HEIGHT = 40;

  struct Item {
    std::string name;
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

  std::vector<FilteredSection> m_filteredSections;
  std::vector<Section> m_sections;

public:
  HorizontalListModel() {
    m_sections.emplace_back(Section{
        .name = "Fruits", .items = {{"banana"}, {"orange"}, {"apple"}, {"pineapple"}, {"strawberry"}}});
    m_sections.emplace_back(
        Section{.name = "Games", .items = {{"Elden Ring"}, {"Black Myth Wukong"}, {"Mario Kart"}}});
    m_sections.emplace_back(
        Section{.name = "Bands", .items = {{"Sabaton"}, {"PowerWolf"}, {"Rigel Theater"}}});

    Section otherSection{.name = "Numbers"};
    int n = 10000;

    otherSection.items.reserve(n);
    for (int i = 0; i != n; ++i) {
      otherSection.items.emplace_back(Item{.name = std::format("Item {}", i)});
    }

    m_sections.emplace_back(otherSection);

    setFilter("");
  }

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
  }

  bool isSectionHeading(Index idx) const {
    return std::holds_alternative<const Section *>(*fromFlatIdx(idx));
  }

  bool isSelectable(Index idx) const override { return !isSectionHeading(idx); }

  void selectionChanged(Index idx, QWidget *widget, bool value) override {
    qDebug() << "selection callback for idx" << idx;
    auto w = static_cast<DefaultListItemWidget *>(widget);
    w->selectionChanged(value);
  }

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

  std::optional<std::variant<const Section *, const std::string *>> fromFlatIdx(Index idx) const {
    int i = 0;
    for (const auto &section : m_filteredSections) {
      if (i == idx) return section.section;
      ++i;
      if (idx >= i && idx < i + section.items.size()) { return &section.items.at(idx - i).item->name; }
      i += section.items.size();
    }

    return std::nullopt;
  }

  StableID stableId(size_t idx) const override {
    auto item = fromFlatIdx(idx);
    const auto visitor = overloads{[&](const std::string *str) { return m_hasher(*str); },
                                   [&](const Section *str) { return m_hasher(str->name); }};
    return std::visit(visitor, item.value());
  }

  size_t height() const override {
    size_t height = 0;
    for (const auto &section : m_filteredSections) {
      height = height + HEADER_HEIGHT + ITEM_HEIGHT * section.items.size();
    }
    return height;
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

  QWidget *createWidget(size_t idx) const override {
    if (isSectionHeading(idx)) { return new TypographyWidget; }
    return new DefaultListItemWidget;
  }

  void refreshWidget(size_t idx, QWidget *widget) const override {
    auto item = fromFlatIdx(idx);
    const auto visitor = overloads{[&](const std::string *str) {
                                     auto w = static_cast<DefaultListItemWidget *>(widget);
                                     w->setName(str->c_str());
                                     w->setIconUrl(ImageURL::builtin("cog"));
                                   },
                                   [&](const Section *str) {
                                     auto w = static_cast<TypographyWidget *>(widget);
                                     return w->setText(str->name.c_str());
                                   }};
    std::visit(visitor, *item);
  }

private:
  std::hash<std::string_view> m_hasher;
};

class SimpleVListStaticModel : public vicinae::ui::VListModel {
  static constexpr const int ITEM_HEIGHT = 40;

public:
  SimpleVListStaticModel() {
    int n = 100;
    m_items.reserve(n);
    for (int i = 0; i != n; ++i) {
      m_items.emplace_back(StandardItem{.title = std::format("Item {}", i), .subtitle = "Fancy subitle"});
    }
    setFilter("");
  }

  void setFilter(const std::string &query) {
    m_filteredItems.reserve(m_items.size());
    m_filteredItems.clear();

    auto matches = [&](const std::string &s) { return s.contains(query); };
    for (const auto &item : m_items | std::views::filter([&](const auto &item) {
                              return matches(item.title) || matches(item.subtitle);
                            })) {
      m_filteredItems.push_back(&item);
    }
  }

  bool isSelectable(Index idx) const override { return true; }

  void selectionChanged(Index idx, QWidget *widget, bool value) override {
    qDebug() << "selection callback for idx" << idx;
    auto w = static_cast<DefaultListItemWidget *>(widget);
    w->selectionChanged(value);
  }

  size_t heightAtIndex(Index idx) const override { return idx * ITEM_HEIGHT; }

  Index indexAtHeight(int height) const override { return height / ITEM_HEIGHT; }

  StableID stableId(size_t idx) const override { return m_hasher(m_filteredItems[idx]->title); }

  size_t height() const override { return m_filteredItems.size() * ITEM_HEIGHT; }

  size_t height(size_t idx) const override { return ITEM_HEIGHT; }

  size_t count() const override { return m_filteredItems.size(); };

  QWidget *createWidget(size_t idx) const override { return new DefaultListItemWidget; }

  void refreshWidget(size_t idx, QWidget *widget) const override {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    auto &item = m_filteredItems[idx];
    w->setName(item->title.c_str());
    w->setSubtitle(item->subtitle);
    w->setIconUrl(ImageURL::builtin("cog"));
  }

private:
  struct StandardItem {
    std::string title;
    std::string subtitle;
  };

  std::hash<std::string_view> m_hasher;
  std::vector<const StandardItem *> m_filteredItems;
  std::vector<StandardItem> m_items;
};

class PlaygroundView : public BaseView {
public:
  PlaygroundView() {
    VStack().add(m_list).imbue(this);
    m_list->setModel(m_model);
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
    m_list->calculate();
    m_list->selectFirst();
  }

private:
  HorizontalListModel *m_model = new HorizontalListModel();
  vicinae::ui::VListWidget *m_list = new vicinae::ui::VListWidget;
};
