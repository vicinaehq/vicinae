#include "extension-grid.hpp"
#include "extension/extension-list.hpp"
#include "layout.hpp"
#include "lib/fuzzy/weighted-fuzzy-scorer.hpp"
#include "ui/omni-grid/omni-grid.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"
#include "ui/image/image.hpp"
#include "ui/color-box/color-box.hpp"

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

class ExtensionGridItem : public OmniGrid::AbstractGridItem {
public:
  void setAspectRatio(double ratio) { m_aspectRatio = ratio; }
  void setFit(ObjectFit fit) { m_fit = fit; }

  const GridItemViewModel &model() const { return _item; }

  ExtensionGridItem(const GridItemViewModel &model, double aspectRatio = 1)
      : _item(model), m_aspectRatio(aspectRatio) {}

private:
  QString generateId() const override { return _item.id; }

  QWidget *centerWidget() const override {
    auto widget = new ExtensionGridContentWidget();
    refreshCenterWidget(widget);
    return widget;
  }

  QString tooltip() const override { return _item.tooltip.value_or(""); }

  QString title() const override { return _item.title; }

  QString subtitle() const override { return _item.subtitle; }

  void recycleCenterWidget(QWidget *widget) const override { refreshCenterWidget(widget); }

  bool centerWidgetRecyclable() const override { return true; }

  void refreshCenterWidget(QWidget *widget) const override {
    auto w = static_cast<ExtensionGridContentWidget *>(widget);
    w->setFit(m_fit);
    w->render(_item.content);
  }

  const QString &name() const { return _item.title; }

  double aspectRatio() const override { return m_aspectRatio; }

  GridItemViewModel _item;
  double m_aspectRatio = 1;
  std::optional<ObjectFit> m_fit;
};

ExtensionGridList::ExtensionGridList() {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_list);
  setLayout(layout);
  m_list->setMargins(20, 10, 20, 10);

  connect(m_list, &OmniList::selectionChanged, this, &ExtensionGridList::handleSelectionChanged);
  connect(m_list, &OmniList::itemActivated, this, &ExtensionGridList::handleItemActivated);
}

void ExtensionGridList::setColumns(int cols) {
  if (m_columns == cols) return;
  m_columns = cols;
  render(OmniList::PreserveSelection);
}

void ExtensionGridList::setAspectRatio(double ratio) { m_aspectRatio = ratio; }
void ExtensionGridList::setFit(ObjectFit fit) { m_fit = fit; }

void ExtensionGridList::setInset(GridItemContentWidget::Inset inset) {
  if (m_inset == inset) return;

  m_inset = inset;
  render(OmniList::PreserveSelection);
}

bool ExtensionGridList::selectUp() { return m_list->selectUp(); }
bool ExtensionGridList::selectDown() { return m_list->selectDown(); }
bool ExtensionGridList::selectLeft() { return m_list->selectLeft(); }
bool ExtensionGridList::selectRight() { return m_list->selectRight(); }
bool ExtensionGridList::selectHome() { return m_list->selectHome(); }
bool ExtensionGridList::selectEnd() { return m_list->selectEnd(); }
void ExtensionGridList::selectNext() { return m_list->selectNext(); }
void ExtensionGridList::activateCurrentSelection() const { m_list->activateCurrentSelection(); }

bool ExtensionGridList::empty() const { return m_list->virtualHeight() == 0; }

void ExtensionGridList::setModel(const std::vector<GridChild> &model, OmniList::SelectionPolicy selection) {
  m_model = model;
  render(selection);
}

void ExtensionGridList::setFilter(const QString &query) {
  if (m_filter == query) return;

  m_filter = query;
  render(OmniList::SelectFirst);
}

GridItemViewModel const *ExtensionGridList::selected() const {
  if (auto selected = m_list->selected()) {
    if (auto qualified = dynamic_cast<ExtensionGridItem const *>(selected)) { return &qualified->model(); }
  }

  return nullptr;
}

int ExtensionGridList::computeItemScore(const GridItemViewModel &item, const std::string &query) {
  static const constexpr double TITLE_WEIGHT = 1;
  static const constexpr double SUBTITLE_WEIGHT = 0.6;
  static const constexpr double KEYWORD_WEIGHT = 0.3;
  fuzzy::WeightedScorer scorer;

  scorer.reserve(2 + item.keywords.size());
  scorer.add(item.title.toStdString(), TITLE_WEIGHT);
  scorer.add(item.subtitle.toStdString(), SUBTITLE_WEIGHT);
  for (const auto &kw : item.keywords) {
    scorer.add(kw.toStdString(), KEYWORD_WEIGHT);
  }
  return scorer.score(query);
}

void ExtensionGridList::render(OmniList::SelectionPolicy selectionPolicy) {
  std::string filter = m_filter.toStdString();

  std::vector<SectionGroup> groups;
  SectionGroup unamedGroup;

  groups.reserve(m_model.size());

  auto associateScore = [&](const GridItemViewModel &model) {
    return std::pair{&model, computeItemScore(model, filter)};
  };
  auto filterScore = [&](auto &&p) { return filter.empty() || p.second > 0; };

  auto handleSingleItem = [&](const GridItemViewModel &model) {
    if (auto p = associateScore(model); filterScore(p)) { unamedGroup.scoredItems.emplace_back(p); }
  };

  auto handleSection = [&](const GridSectionModel &model) {
    if (!unamedGroup.scoredItems.empty()) { groups.emplace_back(std::move(unamedGroup)); }
    SectionGroup grp;
    grp.section = &model;
    grp.scoredItems.reserve(model.children.size());
    auto candidates =
        model.children | std::views::transform(associateScore) | std::views::filter(filterScore);
    for (const auto &item : candidates) {
      grp.bestScore = std::max(grp.bestScore, item.second);
      grp.scoredItems.emplace_back(item);
    }
    groups.emplace_back(grp);
  };

  const auto visitor = overloads{handleSingleItem, handleSection};
  std::ranges::for_each(m_model, [&](auto &&item) { std::visit(visitor, item); });

  if (!unamedGroup.scoredItems.empty()) { groups.emplace_back(unamedGroup); }

  auto buildList = [&]() {
    std::ranges::stable_sort(groups, [](auto &&a, auto &&b) { return a.bestScore > b.bestScore; });
    for (SectionGroup &grp : groups) {
      auto &sec = m_list->addSection(grp.section ? grp.section->title : "");

      if (grp.section && grp.section->title.isEmpty()) { sec.addSpacing(10); }

      sec.setSpacing(10);
      sec.setColumns(grp.section ? grp.section->columns.value_or(m_columns) : m_columns);

      std::ranges::stable_sort(grp.scoredItems, [](auto &&p1, auto &&p2) { return p1.second > p2.second; });

      auto items = grp.scoredItems |
                   std::views::transform([&](auto &&p) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
                     auto gridItem = std::make_unique<ExtensionGridItem>(*p.first);

                     if (grp.section) {
                       gridItem->setFit(grp.section->fit.value_or(m_fit));
                       gridItem->setAspectRatio(grp.section->aspectRatio.value_or(m_aspectRatio));
                       gridItem->setInset(grp.section->inset.value_or(m_inset));
                     } else {
                       gridItem->setFit(m_fit);
                       gridItem->setAspectRatio(m_aspectRatio);
                       gridItem->setInset(m_inset);
                     }

                     return gridItem;
                   });
      sec.addItems(items | std::ranges::to<std::vector>());
    }
  };

  m_list->updateModel(buildList, selectionPolicy);
}

void ExtensionGridList::handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                                               const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    emit selectionChanged(nullptr);
    return;
  }

  if (auto qualifiedNext = dynamic_cast<const ExtensionGridItem *>(next)) {
    emit selectionChanged(&qualifiedNext->model());
  }
}

void ExtensionGridList::handleItemActivated(const OmniList::AbstractVirtualItem &item) {
  if (auto qualified = dynamic_cast<const ExtensionGridItem *>(&item)) {
    emit itemActivated(qualified->model());
  }
}
