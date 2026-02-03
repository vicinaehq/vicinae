#pragma once
#include "common/types.hpp"
#include "extend/grid-model.hpp"
#include "lib/fzf.hpp"
#include "layout.hpp"
#include "ui/color-box/color-box.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"
#include "ui/omni-grid/grid-item-widget.hpp"
#include "ui/vlist/common/grid-model.hpp"

class ExtensionGridContentWidget : public QWidget {
public:
  void render(const GridItemViewModel::Content &content) {
    if (content.index() != m_idx) {
      m_widget = createWidget(content);
      m_idx = content.index();
      HStack().add(m_widget).imbue(this);
    }

    applyWidget(content);
  }

  void setFit(const std::optional<ObjectFit> fit) { m_fit = fit; }

private:
  void applyWidget(const GridItemViewModel::Content &content) {
    const auto visitor =
        overloads{[&](const ImageLikeModel &model) {
                    auto icon = static_cast<ImageWidget *>(m_widget);
                    icon->setObjectFit(m_fit.value_or(ObjectFit::Contain));
                    icon->setUrl(model);
                  },
                  [&](const ColorLike &color) { static_cast<ColorBox *>(m_widget)->setColor(color); }};

    std::visit(visitor, content);
  }

  QWidget *createWidget(const GridItemViewModel::Content &content) {
    const auto visitor = overloads{[](const ColorLike &color) -> QWidget * {
                                     auto widget = new ColorBox();
                                     widget->setBorderRadius(6);
                                     return widget;
                                   },
                                   [](const ImageLikeModel &image) -> QWidget * { return new ImageWidget; }};

    return std::visit(visitor, content);
  }

  std::optional<ObjectFit> m_fit = ObjectFit::Contain;
  QWidget *m_widget = nullptr;
  size_t m_idx = -1;
};

class ExtensionGridModel : public vicinae::ui::GridModel<const GridItemViewModel *> {
public:
  ExtensionGridModel(QObject *parent = nullptr) { setParent(parent); }

  void setData(const std::vector<GridChild> &data) { m_items = data; }

  void reload() { setFilter(m_query); }

  void setColumns(int cols) {
    if (cols == m_columns) return;
    m_columns = cols;
    emit dataChanged();
  }

  void setInset(GridItemContentWidget::Inset inset) {
    if (m_inset == inset) return;
    m_inset = inset;
    emit dataChanged();
  }

  void setFit(ObjectFit fit) { m_fit = fit; }

  void setAspectRatio(double ratio) {
    if (m_aspectRatio == ratio) return;
    m_aspectRatio = ratio;
    emit dataChanged();
  }

  void setFilter(const QString &query) {
    std::string q = query.toStdString();
    SectionData m_anonymousSection;

    m_query = query;
    m_sortedSections.clear();

    const auto toScored = [&](const GridItemViewModel &item) {
      using WS = fzf::WeightedString;
      static const constexpr float TITLE_WEIGHT = 1.0f;
      static const constexpr float SUBTITLE_WEIGHT = 0.6f;
      static const constexpr float KEYWORD_WEIGHT = 0.3f;

      std::initializer_list<WS> fields = {WS{item.title, TITLE_WEIGHT}, WS{item.subtitle, SUBTITLE_WEIGHT}};
      auto kws = item.keywords | std::views::transform([](auto &&s) { return WS{s, KEYWORD_WEIGHT}; });
      auto ss = std::views::concat(fields, kws);
      int score = fzf::defaultMatcher.fuzzy_match_v2_score_query(fields, q, false);

      return ScoredItem{.item = &item, .score = score};
    };

    const auto tryCommitAnonymous = [&]() {
      if (!m_anonymousSection.items.empty()) { m_sortedSections.emplace_back(m_anonymousSection); }
    };

    const auto visitor = overloads{
        [&](const GridItemViewModel &item) {
          if (!m_anonymousSection.items.empty()) { m_anonymousSection.items.reserve(0xFF); }
          if (auto scored = toScored(item); query.isEmpty() || scored.score) {
            m_anonymousSection.bestScore = std::max(m_anonymousSection.bestScore, scored.score);
            m_anonymousSection.items.emplace_back(scored);
          }
        },
        [&](const GridSectionModel &section) {
          tryCommitAnonymous();

          SectionData data{.name = section.title,
                           .columns = section.columns,
                           .aspectRatio = section.aspectRatio,
                           .inset = section.inset,
                           .fit = section.fit};
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

  int sectionColumns(int id) const override { return m_sortedSections[id].columns.value_or(m_columns); }

  double sectionAspectRatio(int id) const override {
    return m_sortedSections[id].aspectRatio.value_or(m_aspectRatio);
  }

  virtual int itemHeight(const GridItemViewModel *const &item, int width, double ratio) const override {
    static GridItemWidget ruler;
    auto fm = ruler.fontMetrics();
    auto spacing = 10;
    int height = width / ratio;

    if (!item->title.empty()) { height += 15 + spacing; }
    if (!item->subtitle.empty()) { height += 15 + spacing; }

    return height;
  }

  int sectionItemCount(int id) const override { return m_sortedSections[id].items.size(); }
  const GridItemViewModel *sectionItemAt(int id, int itemIdx) const override {
    return m_sortedSections[id].items[itemIdx].item;
  }
  int sectionIdFromIndex(int idx) const override { return idx; }
  std::string_view sectionName(int idx) const override { return m_sortedSections[idx].name; }
  WidgetTag widgetTag(const GridItemViewModel *const &item) const override { return 1; }
  WidgetType *createItemWidget(const GridItemViewModel *const &type) const override {
    auto gridContent = new ExtensionGridContentWidget;
    auto w = new GridItemWidget;
    w->setWidget(gridContent);
    return w;
  }
  void refreshItemWidget(const GridItemViewModel *const &type, WidgetType *widget,
                         int sectionId) const override {
    auto w = static_cast<GridItemWidget *>(widget);
    auto content = static_cast<ExtensionGridContentWidget *>(w->widget());
    auto &sec = m_sortedSections[sectionId];

    content->setFit(sec.fit.value_or(m_fit));
    content->render(type->content);
    w->setTitle(type->title.c_str());
    w->setSubtitle(type->subtitle.c_str());
    w->setAspectRatio(sec.aspectRatio.value_or(m_aspectRatio));
    w->setInset(sec.inset.value_or(m_inset));
  }
  StableID stableId(const GridItemViewModel *const &item, int sectionId) const override {
    static std::hash<QString> hasher = {};
    return hasher(item->id.c_str());
  }

private:
  QString m_query;

  int m_columns = 6;
  ObjectFit m_fit = ObjectFit::Contain;
  double m_aspectRatio = 1;
  GridItemContentWidget::Inset m_inset = GridItemContentWidget::Inset::None;

  struct ScoredItem {
    const GridItemViewModel *item;
    int score;
  };

  struct SectionData {
    std::vector<ScoredItem> items;
    std::string name;
    std::optional<int> columns;
    std::optional<double> aspectRatio;
    std::optional<GridItemContentWidget::Inset> inset;
    std::optional<ObjectFit> fit;
    int bestScore;
  };

  std::vector<SectionData> m_sortedSections;
  std::vector<GridChild> m_items;
};
