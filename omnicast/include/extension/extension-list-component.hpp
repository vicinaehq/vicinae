#pragma once
#include "extend/list-model.hpp"
#include <memory>
#include <qdebug.h>
#include "extension/extension-list-detail.hpp"
#include "extension/extension-view.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/omni-list.hpp"
#include "ui/split-detail.hpp"
#include <QJsonArray>
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AppWindow;

class ExtensionListItem : public AbstractDefaultListItem {
  ListItemViewModel _item;

  ItemData data() const override {
    return {
        .iconUrl = _item.icon,
        .name = _item.title,
        .category = _item.subtitle,
    };
  }

  bool hasPartialUpdates() const override { return true; }

  QString generateId() const override { return _item.id; }

public:
  const ListItemViewModel &model() const { return _item; }

  ExtensionListItem(const ListItemViewModel &model) : _item(model) {}
};

class ExtensionList : public QWidget {
  Q_OBJECT

  OmniList *m_list = new OmniList;
  std::vector<ListChild> m_model;
  QString m_filter;

  bool matchesFilter(const ListItemViewModel &item, const QString &query) {
    // todo: improve this
    return item.title.startsWith(query, Qt::CaseInsensitive);
  }

  void render(OmniList::SelectionPolicy selectionPolicy) {
    auto matches = [&](const ListItemViewModel &item) { return matchesFilter(item, m_filter); };
    std::vector<std::shared_ptr<OmniList::AbstractVirtualItem>> currentSectionItems;
    auto appendSectionLess = [&]() {
      if (!currentSectionItems.empty()) {
        auto &listSection = m_list->addSection();

        listSection.addItems(currentSectionItems);
        currentSectionItems.clear();
      }
    };

    m_list->updateModel(
        [&]() {
          for (const auto &item : m_model) {
            if (auto listItem = std::get_if<ListItemViewModel>(&item)) {
              if (!matches(*listItem)) continue;
              currentSectionItems.emplace_back(std::static_pointer_cast<OmniList::AbstractVirtualItem>(
                  std::make_shared<ExtensionListItem>(*listItem)));

            } else if (auto section = std::get_if<ListSectionModel>(&item)) {
              appendSectionLess();

              auto items =
                  section->children | std::views::filter(matches) |
                  std::views::transform([](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
                    return std::make_unique<ExtensionListItem>(item);
                  }) |
                  std::ranges::to<std::vector>();

              if (items.empty()) continue;

              auto &sec = m_list->addSection(section->title);

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

    if (auto qualifiedNext = dynamic_cast<const ExtensionListItem *>(next)) {
      emit selectionChanged(&qualifiedNext->model());
    }
  }

  void handleItemActivated(const OmniList::AbstractVirtualItem &item) {
    if (auto qualified = dynamic_cast<const ExtensionListItem *>(&item)) {
      emit itemActivated(qualified->model());
    }
  }

public:
  ExtensionList() {
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_list);
    setLayout(layout);

    connect(m_list, &OmniList::selectionChanged, this, &ExtensionList::handleSelectionChanged);
    connect(m_list, &OmniList::itemActivated, this, &ExtensionList::handleItemActivated);
  }

  bool selectUp() { return m_list->selectUp(); }
  bool selectDown() { return m_list->selectDown(); }
  void activateCurrentSelection() const { m_list->activateCurrentSelection(); }

  ListItemViewModel const *selected() const {
    if (auto selected = m_list->selected()) {
      if (auto qualified = dynamic_cast<ExtensionListItem const *>(selected)) { return &qualified->model(); }
    }

    return nullptr;
  }

  bool empty() const { return m_list->virtualHeight() == 0; }

  void setModel(const std::vector<ListChild> &model,
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
  void selectionChanged(const ListItemViewModel *);
  void itemActivated(const ListItemViewModel &);
};

class ExtensionListComponent : public ExtensionSimpleView {
  SelectorInput *m_selector = new SelectorInput;
  SplitDetailWidget *m_split = new SplitDetailWidget(this);
  ExtensionListDetail *m_detail = new ExtensionListDetail;
  ListModel _model;
  QVBoxLayout *_layout;
  ExtensionList *m_list = new ExtensionList;
  bool _shouldResetSelection;
  QTimer *_debounce;
  QTimer *m_dropdownDebounce = new QTimer(this);
  bool m_dropdownShouldResetSelection = false;
  int m_renderCount = 0;

  void renderDropdown(const DropdownModel &dropdown);
  void handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item);
  void handleDropdownSearchChanged(const QString &text);

  QWidget *searchBarAccessory() const override { return m_selector; }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Up:
      return m_list->selectUp();
    case Qt::Key_Down:
      return m_list->selectDown();
    case Qt::Key_Return:
      m_list->activateCurrentSelection();
      return true;
    }

    return ExtensionSimpleView::inputFilter(event);
  }

public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const ListItemViewModel *next);
  void onItemActivated(const ListItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;

  ExtensionListComponent();
};
