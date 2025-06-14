#pragma once
#include "extend/grid-model.hpp"
#include <qdebug.h>
#include "extension/extension-view.hpp"
#include "omni-icon.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <QJsonArray>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qtimer.h>

class ExtensionGridItem : public OmniGrid::AbstractGridItem {
  GridItemViewModel _item;
  double m_aspectRatio = 1;

  QString generateId() const override { return _item.id; }

  QWidget *centerWidget() const override {
    auto icon = new Omnimg::ImageWidget;

    icon->setUrl(_item.content);

    return icon;
  }

  void recycleCenterWidget(QWidget *widget) const override { refreshCenterWidget(widget); }

  bool centerWidgetRecyclable() const override { return true; }

  void refreshCenterWidget(QWidget *widget) const override {
    auto icon = static_cast<Omnimg::ImageWidget *>(widget);

    icon->setUrl(_item.content);
  }

  const QString &name() const { return _item.title; }

  double aspectRatio() const override { return m_aspectRatio; }

public:
  const GridItemViewModel &model() const { return _item; }

  ExtensionGridItem(const GridItemViewModel &model, double aspectRatio = 1)
      : _item(model), m_aspectRatio(aspectRatio) {}
};

class ExtensionGridList : public QWidget {
  Q_OBJECT

  OmniList *m_list = new OmniList;
  std::vector<GridChild> m_model;
  int m_columns = 1;
  QString m_filter;

  bool matchesFilter(const GridItemViewModel &item, const QString &query) {
    bool keywordMatches = std::ranges::any_of(
        item.keywords, [&](auto &&keyword) { return keyword.contains(query, Qt::CaseInsensitive); });

    return keywordMatches || item.title.contains(query, Qt::CaseInsensitive);
  }

  void render(OmniList::SelectionPolicy selectionPolicy) {
    auto matches = [&](const GridItemViewModel &item) { return matchesFilter(item, m_filter); };
    std::vector<std::shared_ptr<OmniList::AbstractVirtualItem>> currentSectionItems;
    auto appendSectionLess = [&]() {
      if (!currentSectionItems.empty()) {
        auto &listSection = m_list->addSection();

        listSection.setColumns(m_columns);
        listSection.addItems(currentSectionItems);
        currentSectionItems.clear();
      }
    };

    qDebug() << "UPDATED MODEL";
    m_list->updateModel(
        [&]() {
          for (const auto &item : m_model) {
            if (auto listItem = std::get_if<GridItemViewModel>(&item)) {
              if (!matches(*listItem)) continue;
              currentSectionItems.emplace_back(std::static_pointer_cast<OmniList::AbstractVirtualItem>(
                  std::make_shared<ExtensionGridItem>(*listItem)));

            } else if (auto section = std::get_if<GridSectionModel>(&item)) {
              appendSectionLess();

              auto items =
                  section->children | std::views::filter(matches) |
                  std::views::transform([&](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
                    return std::make_unique<ExtensionGridItem>(item, section->aspectRatio);
                  }) |
                  std::ranges::to<std::vector>();

              if (items.empty()) continue;

              auto &sec = m_list->addSection(section->title);

              if (section->title.isEmpty()) { sec.addSpacing(10); }

              sec.setSpacing(10);
              sec.setColumns(section->columns);
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

  bool selectUp() { return m_list->selectUp(); }
  bool selectDown() { return m_list->selectDown(); }
  bool selectLeft() { return m_list->selectLeft(); }
  bool selectRight() { return m_list->selectRight(); }
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
};

class ExtensionGridComponent : public ExtensionSimpleView {
  GridModel _model;
  ExtensionGridList *m_list = new ExtensionGridList;
  bool _shouldResetSelection;
  QTimer *_debounce;

public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const GridItemViewModel *item);
  void onItemActivated(const GridItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;

  ExtensionGridComponent();
};
