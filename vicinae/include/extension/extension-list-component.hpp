#pragma once
#include <memory>
#include <qdebug.h>
#include <QJsonArray>
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qstackedwidget.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "extend/list-model.hpp"
#include "extension/extension-list-detail.hpp"
#include "extension/extension-view.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/split-detail/split-detail.hpp"

class ExtensionListItem : public AbstractDefaultListItem {
public:
  const ListItemViewModel &model() const { return _item; }

  ExtensionListItem(const ListItemViewModel &model) : _item(model) {}

private:
  AccessoryList accessories() const {
    AccessoryList list;

    list.reserve(_item.accessories.size());
    for (const auto &accessory : _item.accessories) {
      list.emplace_back(accessory.toAccessory());
    }

    return list;
  }

  ItemData data() const override {
    return {
        .iconUrl = _item.icon, .name = _item.title, .subtitle = _item.subtitle, .accessories = accessories()};
  }

  bool hasPartialUpdates() const override { return true; }

  QString generateId() const override { return _item.id; }

  ListItemViewModel _item;
};

class ExtensionList : public QWidget {
  Q_OBJECT

  OmniList *m_list = new OmniList;
  std::vector<ListChild> m_model;
  QString m_filter;

  bool matchesFilter(const ListItemViewModel &item, const QString &query) {
    auto pred = [&](const QString &kw) { return kw.contains(query, Qt::CaseInsensitive); };

    // TODO: use better search algorithm if we run into issues
    return pred(item.title) || pred(item.subtitle) || std::ranges::any_of(item.keywords, pred);
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

              auto &sec = m_list->addSection(section->title);

              for (const auto &item : section->children | std::views::filter(matches)) {
                sec.addItem(std::make_unique<ExtensionListItem>(item));
              }
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
  bool selectHome() { return m_list->selectHome(); }
  bool selectEnd() { return m_list->selectEnd(); }
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
public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const ListItemViewModel *next);
  void onItemActivated(const ListItemViewModel &item);
  void handleDebouncedSearchNotification();
  void textChanged(const QString &text) override;
  void initialize() override;

  ExtensionListComponent();
  ~ExtensionListComponent();

private:
  void renderDropdown(const DropdownModel &dropdown);
  void handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item);
  void handleDropdownSearchChanged(const QString &text);

  QWidget *searchBarAccessory() const override { return m_selector; }
  bool inputFilter(QKeyEvent *event) override;

  QStackedWidget *m_content = new QStackedWidget(this);
  EmptyViewWidget *m_emptyView = new EmptyViewWidget(this);
  SelectorInput *m_selector = new SelectorInput(this);
  SplitDetailWidget *m_split = new SplitDetailWidget(this);
  ExtensionListDetail *m_detail = new ExtensionListDetail;
  ListModel _model;
  ExtensionList *m_list = new ExtensionList;
  bool _shouldResetSelection;
  QTimer *_debounce;
  QTimer *m_dropdownDebounce = new QTimer(this);
  bool m_dropdownShouldResetSelection = false;
  int m_renderCount = 0;
};
