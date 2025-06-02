#include "extension/extension-list-component.hpp"
#include "extend/list-model.hpp"
#include "extension/extension-list-detail.hpp"
#include <chrono>
#include <qcoreevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <ranges>
#include <sched.h>
#include "extension/extension-view.hpp"
#include "ui/form/app-picker-input.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/omni-list.hpp"

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};
/*
class ListDropdownSelectorItem : public SelectorInput::AbstractItem {
  DropdownModel::Item m_model;

  QString generateId() const override { return m_model.value; }

  OmniIconUrl icon() const override {
    return m_model.icon ? OmniIconUrl(*m_model.icon) : BuiltinOmniIconUrl("circle");
  }

#include "omni-icon.hpp"
  QString displayName() const override { return m_model.title; }

  AbstractItem *clone() const override { return new DropdownSelectorItem(*this); }

  const DropdownModel::Item &item() const { return m_model; }

public:
  ListDropdownSelectorItem(const DropdownModel::Item &model) : m_model(model) {}
};
*/

bool ExtensionListComponent::inputFilter(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Left:
  case Qt::Key_Right:
  case Qt::Key_Up:
  case Qt::Key_Down:
  case Qt::Key_Return:
    QApplication::sendEvent(_list, event);
    return true;
  }

  return ExtensionSimpleView::inputFilter(event);
}

void ExtensionListComponent::renderDropdown(const DropdownModel &dropdown) {
  qWarning() << "RENDERING DROPDOWN!";
  OmniList::SelectionPolicy selectionPolicy = OmniList::PreserveSelection;

  m_dropdownDebounce->setInterval(dropdown.throttle ? THROTTLE_DEBOUNCE_DURATION
                                                    : std::chrono::milliseconds(0));

  if (m_dropdownShouldResetSelection) {
    m_dropdownShouldResetSelection = false;
    selectionPolicy = OmniList::SelectFirst;
  }

  auto list = m_selector->list();

  list->updateModel(
      [&]() {
        OmniList::Section *currentSection = nullptr;

        for (const auto &item : dropdown.children) {
          if (auto listItem = std::get_if<DropdownModel::Item>(&item)) {
            if (!currentSection) { currentSection = &list->addSection(); }
            currentSection->addItem(std::make_unique<DropdownSelectorItem>(*listItem));
          } else if (auto section = std::get_if<DropdownModel::Section>(&item)) {
            auto mapItem = [](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
              return std::make_unique<DropdownSelectorItem>(item);
            };
            auto items = section->items | std::views::transform(mapItem) | std::ranges::to<std::vector>();
            auto &sec = list->addSection(section->title);

            sec.addItems(std::move(items));
          }
        }
      },
      selectionPolicy);

  m_selector->setEnableDefaultFilter(dropdown.filtering.enabled);

  if (auto controlledValue = dropdown.value) {
    m_selector->setValue(*controlledValue);
  } else if (!m_selector->value()) {
    if (dropdown.defaultValue) {
      m_selector->setValue(*dropdown.defaultValue);
    } else if (auto item = m_selector->list()->firstSelectableItem()) {
      m_selector->setValue(item->generateId());
    }
  }

  m_selector->setIsLoading(dropdown.isLoading);
}

void ExtensionListComponent::render(const RenderModel &baseModel) {
  ++m_renderCount;
  qCritical() << "render" << "count" << m_renderCount;
  auto newModel = std::get<ListModel>(baseModel);

  if (auto accessory = newModel.searchBarAccessory) {
    auto dropdown = std::get<DropdownModel>(*accessory);

    renderDropdown(dropdown);
  }

  qCritical() << "list dirty" << newModel.dirty;

  m_selector->setVisible(newModel.searchBarAccessory.has_value() && isVisible());

  if (!newModel.navigationTitle.isEmpty()) {
    qDebug() << "set navigation title" << newModel.navigationTitle;
    setNavigationTitle(newModel.navigationTitle);
  }
  if (!newModel.searchPlaceholderText.isEmpty()) { setSearchPlaceholderText(newModel.searchPlaceholderText); }

  if (auto text = newModel.searchText) {
    qDebug() << "[DEBUG] SET SEARCH TEXT" << text;
    setSearchText(*text);
  }

  if (newModel.throttle != _model.throttle) {
    _debounce->stop();

    if (newModel.throttle) {
      _debounce->setInterval(THROTTLE_DEBOUNCE_DURATION);
    } else {
      _debounce->setInterval(0);
    }
  }

  setLoading(newModel.isLoading);

  if (newModel.dirty) {
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

    items.reserve(newModel.items.size());

    int i = 0;

    OmniList::SelectionPolicy policy = OmniList::SelectFirst;

    if (_shouldResetSelection) {
      qDebug() << "should reset selection";
      _shouldResetSelection = false;
      policy = OmniList::SelectFirst;
    } else {
      policy = OmniList::PreserveSelection;
    }

    _list->updateModel(
        [&]() {
          for (const auto &item : newModel.items) {
            if (auto listItem = std::get_if<ListItemViewModel>(&item)) {
              // items.push_back(std::make_unique<ExtensionListItem>(*listItem));
            } else if (auto section = std::get_if<ListSectionModel>(&item)) {
              if (section->children.empty()) continue;

              auto &sec = _list->addSection(section->title);
              auto items =
                  section->children |
                  std::views::transform([](auto &&item) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
                    return std::make_unique<ExtensionListItem>(item);
                  }) |
                  std::ranges::to<std::vector>();

              sec.addItems(std::move(items));
            }
          }
        },
        policy);
  }

  if (!newModel.searchText) {
    if (_shouldResetSelection) {
      if (newModel.filtering) {
        _list->setFilter(std::make_unique<BuiltinExtensionItemFilter>(searchText()));
      } else {
        _list->clearFilter();
      }
    }
  }

  _model = newModel;

  if (auto selected = _list->selected()) {
    auto item = static_cast<const ExtensionListItem *>(selected);

    m_split->setDetailVisibility(item->model().detail.has_value() && _model.isShowingDetail);

    if (auto detail = item->model().detail) {
      if (m_split->isDetailVisible()) {
        qDebug() << "update detail for" << selected->generateId();
        m_detail->updateDetail(*detail);
      } else {
        qDebug() << "create detail";
        m_detail->setDetail(*detail);
      }
    }
  }

  if (_list->virtualHeight() == 0) {
    qDebug() << "is empty!";
    size_t i = 0;

    if (auto pannel = newModel.actions) {
      for (auto &item : pannel->children) {
        if (auto action = std::get_if<ActionModel>(&item)) {
          if (i == 0) action->shortcut = primaryShortcut;
          if (i == 1) action->shortcut = secondaryShortcut;

          ++i;
        }
      }

      setActionPanel(*pannel);
    }
  }
}

void ExtensionListComponent::onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                                                const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    qDebug() << "nore visibiliy breaux";
    m_split->setDetailVisibility(false);

    if (auto &pannel = _model.actions) {
      setActionPanel(*pannel);
    } else {
      setActionPanel({});
    }
    return;
  }

  qDebug() << "set visibility of" << next->generateId() << _model.isShowingDetail;

  auto item = static_cast<const ExtensionListItem *>(next);
  size_t i = 0;

  m_split->setDetailVisibility(_model.isShowingDetail && item->model().detail);

  if (auto detail = item->model().detail) {
    qDebug() << "set markdown for" << next->generateId();
    m_detail->setDetail(*detail);
  }

  if (auto pannel = item->model().actionPannel) {
    for (auto &item : pannel->children) {
      if (auto action = std::get_if<ActionModel>(&item)) {
        if (i == 0) action->shortcut = primaryShortcut;
        if (i == 1) action->shortcut = secondaryShortcut;

        ++i;
      }
    }

    setActionPanel(*pannel);
  }

  if (auto handler = _model.onSelectionChanged) { notify(*handler, {next->generateId()}); }
}

void ExtensionListComponent::handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item) {
  if (auto accessory = _model.searchBarAccessory) {
    if (auto dropdown = std::get_if<DropdownModel>(&*accessory)) {
      if (auto onChange = dropdown->onChange) { notify(*onChange, {item.generateId()}); }
    }
  }
}

void ExtensionListComponent::handleDropdownSearchChanged(const QString &text) {

  if (auto accessory = _model.searchBarAccessory) {
    if (auto dropdown = std::get_if<DropdownModel>(&*accessory)) {
      m_dropdownShouldResetSelection = !dropdown->filtering.enabled;

      if (auto onChange = dropdown->onSearchTextChange) { emit notify(*onChange, {text}); }
    }
  }
}

void ExtensionListComponent::handleDebouncedSearchNotification() {
  auto text = searchText();

  if (_model.filtering) {
    _list->setFilter(std::make_unique<BuiltinExtensionItemFilter>(text));
  } else {
    _list->clearFilter();
  }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;

    qDebug() << "[DEBUG] sending search changed event" << text;

    notify(*handler, {text});
  }
}

void ExtensionListComponent::onItemActivated(const OmniList::AbstractVirtualItem &item) {
  // TODO: activate item
  // selectPrimaryAction();
}

void ExtensionListComponent::onSearchChanged(const QString &text) { _debounce->start(); }

ExtensionListComponent::ExtensionListComponent()
    : _debounce(new QTimer(this)), _layout(new QVBoxLayout), _list(new OmniList),
      _shouldResetSelection(true) {
  m_selector->setMinimumWidth(400);
  m_selector->setEnableDefaultFilter(false);
  m_topBar->setAccessoryWidget(m_selector);
  m_selector->hide();

  m_split->setMainWidget(_list);
  m_split->setDetailWidget(m_detail);
  setupUI(m_split);

  _debounce->setSingleShot(true);
  connect(_debounce, &QTimer::timeout, this, &ExtensionListComponent::handleDebouncedSearchNotification);
  connect(_list, &OmniList::selectionChanged, this, &ExtensionListComponent::onSelectionChanged);
  connect(_list, &OmniList::itemActivated, this, &ExtensionListComponent::onItemActivated);
  connect(m_selector, &SelectorInput::selectionChanged, this,
          &ExtensionListComponent::handleDropdownSelectionChanged);
  connect(m_selector, &SelectorInput::textChanged, this,
          &ExtensionListComponent::handleDropdownSearchChanged);
}
