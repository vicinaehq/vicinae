#pragma once
#include <ranges>
#include "common/types.hpp"
#include "extend/list-model.hpp"
#include "lib/fzf.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/vlist/common/section-model.hpp"

class ExtensionListModel : public vicinae::ui::SectionListModel<const ListItemViewModel *> {
public:
  ExtensionListModel(QObject *parent = nullptr) { setParent(parent); }

  void setData(const std::vector<ListChild> &data) { m_items = data; }

  void reload() { setFilter(m_query); }

  void setFilter(const QString &query) {
    std::string q = query.toStdString();

    m_query = query;
    m_sortedSections.clear();

    SectionData m_anonymousSection;

    const auto toScored = [&](const ListItemViewModel &item) {
      static const constexpr float TITLE_WEIGHT = 1.0f;
      static const constexpr float SUBTITLE_WEIGHT = 0.6f;
      static const constexpr float KEYWORD_WEIGHT = 0.3f;

      using WS = fzf::WeightedString;
      std::initializer_list<WS> fields = {WS{item.title, TITLE_WEIGHT}, WS{item.subtitle, SUBTITLE_WEIGHT}};
      auto kws = item.keywords | std::views::transform([](auto &&s) { return WS{s, KEYWORD_WEIGHT}; });
      auto ss = std::views::concat(fields, kws);
      int score = fzf::defaultMatcher.fuzzy_match_v2_score_query(ss, q);

      return ScoredItem{.item = &item, .score = score};
    };

    const auto tryCommitAnonymous = [&]() {
      if (!m_anonymousSection.items.empty()) { m_sortedSections.emplace_back(m_anonymousSection); }
    };

    const auto visitor = overloads{
        [&](const ListItemViewModel &item) {
          if (!m_anonymousSection.items.empty()) { m_anonymousSection.items.reserve(0xFF); }
          if (auto scored = toScored(item); query.isEmpty() || scored.score) {
            m_anonymousSection.bestScore = std::max(m_anonymousSection.bestScore, scored.score);
            m_anonymousSection.items.emplace_back(scored);
          }
        },
        [&](const ListSectionModel &section) {
          tryCommitAnonymous();

          SectionData data{.name = section.title};
          data.items.reserve(section.children.size());

          for (const auto &item : section.children) {
            if (auto scored = toScored(item); query.isEmpty() || scored.score) {
              data.bestScore = std::max(data.bestScore, scored.score);
              data.items.emplace_back(scored);
            }
          }

          m_sortedSections.emplace_back(data);
        },
    };

    for (const auto &item : m_items) {
      std::visit(visitor, item);
    }

    tryCommitAnonymous();

    for (SectionData &section : m_sortedSections) {
      std::ranges::stable_sort(section.items, [](auto &&a, auto &&b) { return a.score > b.score; });
    }
    std::ranges::stable_sort(m_sortedSections, [](auto &&a, auto &&b) { return a.bestScore > b.bestScore; });

    emit dataChanged();
  }

protected:
  int sectionCount() const override { return m_sortedSections.size(); }
  int sectionItemCount(int id) const override { return m_sortedSections[id].items.size(); }
  int sectionItemHeight(int id) const override { return 41; }
  const ListItemViewModel *sectionItemAt(int id, int itemIdx) const override {
    return m_sortedSections[id].items[itemIdx].item;
  }
  int sectionIdFromIndex(int idx) const override { return idx; }
  std::string_view sectionName(int idx) const override { return m_sortedSections[idx].name; }
  WidgetTag widgetTag(const ListItemViewModel *const &item) const override { return 1; }
  WidgetType *createItemWidget(const ListItemViewModel *const &type) const override {
    return new DefaultListItemWidget;
  }
  void refreshItemWidget(const ListItemViewModel *const &type, WidgetType *widget) const override {
    auto w = static_cast<DefaultListItemWidget *>(widget);
    w->setName(type->title.c_str());
    w->setSubtitle(type->subtitle);
    w->setIconUrl(type->icon);
    w->setAccessories(makeAccessoryList(*type));
    w->setActive(false);
  }
  StableID stableId(const ListItemViewModel *const &item) const override {
    static std::hash<QString> hasher = {};
    return hasher(item->id.c_str());
  }

private:
  static AccessoryList makeAccessoryList(const ListItemViewModel &item) {
    AccessoryList list;
    list.reserve(item.accessories.size());
    for (const auto &accessory : item.accessories) {
      list.emplace_back(accessory.toAccessory());
    }
    return list;
  }
  QString m_query;

  struct ScoredItem {
    const ListItemViewModel *item;
    int score;
  };

  struct SectionData {
    std::vector<ScoredItem> items;
    std::string name;
    int bestScore;
  };

  std::vector<SectionData> m_sortedSections;
  std::vector<ListChild> m_items;
};
