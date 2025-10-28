#pragma once
#include <qdebug.h>
#include <QJsonArray>
#include <qboxlayout.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qresource.h>
#include <qstackedwidget.h>
#include <qtimer.h>
#include <qwidget.h>
#include "extend/grid-model.hpp"
#include "extend/image-model.hpp"
#include "extension/extension-view.hpp"
#include "ui/color-box/color-box.hpp"
#include "ui/image/url.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"
#include "ui/image/image.hpp"
#include "ui/omni-grid/omni-grid.hpp"
#include "ui/omni-list/omni-list.hpp"

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

class ExtensionGridList : public QWidget {
  Q_OBJECT

public:
  ExtensionGridList() {
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_list);
    setLayout(layout);
    m_list->setMargins(20, 10, 20, 10);

    connect(m_list, &OmniList::selectionChanged, this, &ExtensionGridList::handleSelectionChanged);
    connect(m_list, &OmniList::itemActivated, this, &ExtensionGridList::handleItemActivated);
  }

  void setColumns(int cols) {
    if (m_columns == cols) return;
    m_columns = cols;
    render(OmniList::PreserveSelection);
  }

  void setAspectRatio(double ratio) { m_aspectRatio = ratio; }
  void setFit(ObjectFit fit) { m_fit = fit; }

  void setInset(GridItemContentWidget::Inset inset) {
    if (m_inset == inset) return;

    m_inset = inset;
    render(OmniList::PreserveSelection);
  }

  bool selectUp() { return m_list->selectUp(); }
  bool selectDown() { return m_list->selectDown(); }
  bool selectLeft() { return m_list->selectLeft(); }
  bool selectRight() { return m_list->selectRight(); }
  bool selectHome() { return m_list->selectHome(); }
  bool selectEnd() { return m_list->selectEnd(); }
  void activateCurrentSelection() const { m_list->activateCurrentSelection(); }

  GridItemViewModel const *selected() const {
    if (auto selected = m_list->selected()) {
      if (auto qualified = dynamic_cast<ExtensionGridItem const *>(selected)) { return &qualified->model(); }
    }

    return nullptr;
  }

  bool empty() const { return m_list->virtualHeight() == 0; }

  void setModel(const std::vector<GridChild> &model,
                OmniList::SelectionPolicy selection = OmniList::SelectFirst) {
    m_model = model;
    render(selection);
  }
  void setFilter(const QString &query) {
    if (m_filter == query) return;

    m_filter = query;
    render(OmniList::SelectFirst);
  }

signals:
  void selectionChanged(const GridItemViewModel *);
  void itemActivated(const GridItemViewModel &);

private:
  bool matchesFilter(const GridItemViewModel &item, const QString &query) {
    auto pred = [&](const QString &text) { return text.contains(query, Qt::CaseInsensitive); };
    return pred(item.title) || pred(item.subtitle) || std::ranges::any_of(item.keywords, pred);
  }

  void render(OmniList::SelectionPolicy selectionPolicy) {
    auto matches = [&](const GridItemViewModel &item) { return matchesFilter(item, m_filter); };
    std::vector<std::shared_ptr<OmniList::AbstractVirtualItem>> currentSectionItems;
    auto appendSectionLess = [&]() {
      if (!currentSectionItems.empty()) {
        auto &listSection = m_list->addSection();

        listSection.setSpacing(10);
        listSection.setColumns(m_columns);
        listSection.addItems(currentSectionItems);
        currentSectionItems.clear();
      }
    };

    m_list->updateModel(
        [&]() {
          for (const auto &item : m_model) {
            if (auto listItem = std::get_if<GridItemViewModel>(&item)) {
              if (!matches(*listItem)) continue;

              auto item = std::make_shared<ExtensionGridItem>(*listItem);

              item->setFit(m_fit);
              item->setAspectRatio(m_aspectRatio);
              item->setInset(m_inset);
              currentSectionItems.emplace_back(std::static_pointer_cast<OmniList::AbstractVirtualItem>(item));
            } else if (auto section = std::get_if<GridSectionModel>(&item)) {
              appendSectionLess();
              std::vector<std::shared_ptr<OmniList::AbstractVirtualItem>> items;

              items.reserve(section->children.size());

              for (auto &item : section->children | std::views::filter(matches)) {
                auto gridItem = std::make_unique<ExtensionGridItem>(item);

                gridItem->setFit(section->fit.value_or(m_fit));
                gridItem->setAspectRatio(section->aspectRatio.value_or(m_aspectRatio));
                gridItem->setInset(section->inset.value_or(m_inset));
                items.emplace_back(std::move(gridItem));
              }

              if (items.empty()) continue;

              auto &sec = m_list->addSection(section->title);

              if (section->title.isEmpty()) { sec.addSpacing(10); }

              sec.setSpacing(10);
              sec.setColumns(section->columns.value_or(m_columns));
              sec.addItems(std::move(items));
            }
          }
          appendSectionLess();
        },
        selectionPolicy);
  }

  void handleSelectionChanged(const OmniList::AbstractVirtualItem *next,
                              const OmniList::AbstractVirtualItem *previous) {
    if (!next) {
      emit selectionChanged(nullptr);
      return;
    }

    if (auto qualifiedNext = dynamic_cast<const ExtensionGridItem *>(next)) {
      emit selectionChanged(&qualifiedNext->model());
    }
  }

  void handleItemActivated(const OmniList::AbstractVirtualItem &item) {
    if (auto qualified = dynamic_cast<const ExtensionGridItem *>(&item)) {
      emit itemActivated(qualified->model());
    }
  }

  OmniList *m_list = new OmniList;
  std::vector<GridChild> m_model;
  int m_columns = 1;
  double m_aspectRatio = 1;
  ObjectFit m_fit = ObjectFit::Contain;
  GridItemContentWidget::Inset m_inset = GridItemContentWidget::Inset::None;
  QString m_filter;
};

class ExtensionGridComponent : public ExtensionSimpleView {
public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const GridItemViewModel *item);
  void onItemActivated(const GridItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;

  ExtensionGridComponent();

private:
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);
  QStackedWidget *m_content = new QStackedWidget(this);
  GridModel _model;
  ExtensionGridList *m_list = new ExtensionGridList;
  bool _shouldResetSelection;
  QTimer *_debounce;
};
