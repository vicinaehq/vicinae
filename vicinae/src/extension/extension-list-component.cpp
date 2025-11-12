#include "extension/extension-list-component.hpp"
#include "extend/list-model.hpp"
#include "extension/extension-list-detail.hpp"
#include <chrono>
#include <memory>
#include <qcoreevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include "extension/form/extension-dropdown.hpp"
#include "keyboard/keybind-manager.hpp"
#include "ui/form/app-picker-input.hpp"
#include "ui/form/selector-input.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "services/config/config-service.hpp"
#include "service-registry.hpp"

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

void ExtensionListComponent::renderDropdown(const DropdownModel &dropdown) {
  OmniList::SelectionPolicy selectionPolicy = OmniList::PreserveSelection;

  m_dropdownDebounce->setInterval(dropdown.throttle ? THROTTLE_DEBOUNCE_DURATION
                                                    : std::chrono::milliseconds(0));

  if (dropdown.dirty) {
    if (m_dropdownShouldResetSelection) {
      m_dropdownShouldResetSelection = false;
      selectionPolicy = OmniList::SelectFirst;
    }

    m_selector->resetModel();

    std::vector<std::shared_ptr<SelectorInput::AbstractItem>> freeSectionItems;

    for (const auto &item : dropdown.children) {
      if (auto listItem = std::get_if<DropdownModel::Item>(&item)) {
        freeSectionItems.emplace_back(std::make_shared<DropdownSelectorItem>(*listItem));
      } else if (auto section = std::get_if<DropdownModel::Section>(&item)) {
        if (!freeSectionItems.empty()) {
          // m_selector->addSection("", freeSectionItems);
          freeSectionItems.clear();
        }

        std::vector<std::shared_ptr<SelectorInput::AbstractItem>> items;

        for (const auto &item : section->items) {
          items.emplace_back(std::make_unique<DropdownSelectorItem>(item));
        }

        m_selector->addSection(section->title, items);
      }
    }

    if (!freeSectionItems.empty()) {
      m_selector->addSection("", freeSectionItems);
      freeSectionItems.clear();
    }

    m_selector->updateModel();
  }

  m_selector->setEnableDefaultFilter(dropdown.filtering.enabled);

  // Track if we are setting an initial value so we can fire onChange once on first load
  const bool hadPreviousSelection = m_selector->value() != nullptr;

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

  // If there was no previous selection and we just set one, invoke onChange once
  if (!hadPreviousSelection) {
    if (auto selected = m_selector->value()) {
      if (auto onChange = dropdown.onChange) { notify(*onChange, {selected->generateId()}); }
    }
  }
}

bool ExtensionListComponent::inputFilter(QKeyEvent *event) {
  auto config = ServiceRegistry::instance()->config();
  const QString keybinding = config->value().keybinding;

  if (event->modifiers() == Qt::ControlModifier) {
    if (KeyBindingService::isDownKey(event, keybinding)) { return m_list->selectDown(); }
    if (KeyBindingService::isUpKey(event, keybinding)) { return m_list->selectUp(); }
  }

  if (event->modifiers().toInt() == 0) {
    switch (event->key()) {
    case Qt::Key_Up:
      return m_list->selectUp();
    case Qt::Key_Down:
      return m_list->selectDown();
    case Qt::Key_Home:
      return m_list->selectHome();
    case Qt::Key_Tab:
      if (!context()->navigation->hasCompleter()) {
        m_list->selectNext();
        return true;
      }
      break;
    case Qt::Key_End:
      return m_list->selectEnd();
    }
  }

  if (m_selector->isVisible() &&
      KeybindManager::instance()->resolve(Keybind::OpenSearchAccessorySelector) == event) {
    m_selector->openSelector();
    return true;
  }

  return ExtensionSimpleView::inputFilter(event);
}

void ExtensionListComponent::render(const RenderModel &baseModel) {
  ++m_renderCount;
  auto newModel = std::get<ListModel>(baseModel);

  if (auto accessory = newModel.searchBarAccessory) {
    auto dropdown = std::get<DropdownModel>(*accessory);

    renderDropdown(dropdown);
  }

  setSearchAccessoryVisiblity(newModel.searchBarAccessory.has_value() && isVisible());

  if (!newModel.navigationTitle.isEmpty()) { setNavigationTitle(newModel.navigationTitle); }
  if (!newModel.searchPlaceholderText.isEmpty()) { setSearchPlaceholderText(newModel.searchPlaceholderText); }
  if (auto text = newModel.searchText) { setSearchText(*text); }

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

    OmniList::SelectionPolicy policy = OmniList::SelectFirst;

    if (_shouldResetSelection) {
      _shouldResetSelection = false;
      policy = OmniList::SelectFirst;
    } else {
      policy = OmniList::PreserveSelection;
    }

    m_list->setModel(newModel.items, policy);
  }

  /*
  if (!newModel.searchText) {
    if (_shouldResetSelection) {
      if (newModel.filtering) {
        m_list->setFilter(searchText());
      } else {
        m_list->setFilter("");
      }
    }
  }
  */

  _model = newModel;

  if (auto selected = m_list->selected(); selected && newModel.dirty) {
    if (auto detail = selected->detail) {
      m_split->detailWidget()->show();
      if (m_split->isDetailVisible()) {
        m_detail->updateDetail(*detail);
      } else {
        m_detail->setDetail(*detail);
      }
    } else {
      m_split->detailWidget()->hide();
    }

    if (auto panel = selected->actionPannel) {
      if (_model.dirty && panel->dirty) { setActionPanel(*panel); }
    } else {
      clearActions();
    }

    m_split->setDetailVisibility(_model.isShowingDetail);
  }

  if (auto empty = newModel.emptyView) {
    m_emptyView->setTitle(empty->title);
    m_emptyView->setDescription(empty->description);

    if (auto icon = empty->icon) {
      m_emptyView->setIcon(*icon);
    } else {
      m_emptyView->setIcon(ImageURL::builtin("magnifying-glass"));
    }
  }

  if (m_list->empty()) {
    if (auto panel = newModel.actions) {
      if (panel->dirty) { setActionPanel(*panel); }
    } else {
      clearActions();
    }

    if (auto empty = newModel.emptyView) {
      m_content->setCurrentWidget(m_emptyView);
    } else {
      m_content->setCurrentWidget(m_split);
    }
  } else {
    m_content->setCurrentWidget(m_split);
  }
}

void ExtensionListComponent::onSelectionChanged(const ListItemViewModel *next) {
  if (!next) {
    m_split->setDetailVisibility(false);

    if (auto &pannel = _model.actions) {
      setActionPanel(*pannel);
    } else {
      clearActions();
    }
    return;
  }

  if (auto handler = _model.onSelectionChanged) { notify(*handler, {next->id}); }

  m_split->setDetailVisibility(_model.isShowingDetail);

  if (auto detail = next->detail) { m_detail->setDetail(*detail); }

  if (auto pannel = next->actionPannel) {
    setActionPanel(*pannel);
  } else {
    clearActions();
  }
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
    m_list->setFilter(searchText());
  } else {
    m_list->setFilter("");
  }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;

    notify(*handler, {text});
  }

  if (m_list->empty()) {
    if (_model.emptyView) { m_content->setCurrentWidget(m_emptyView); }
  } else {
    m_content->setCurrentWidget(m_split);
  }
}

void ExtensionListComponent::onItemActivated(const ListItemViewModel &item) { executePrimaryAction(); }

void ExtensionListComponent::textChanged(const QString &text) {
  if (text == _model.searchText) { /*m_topBar->input->setText(*_model.searchText);*/
    return;
  }

  if (_model.throttle) {
    _debounce->start();
  } else {
    handleDebouncedSearchNotification();
  }
}

void ExtensionListComponent::initialize() { setSearchAccessoryVisiblity(false); }

ExtensionListComponent::ExtensionListComponent() : _debounce(new QTimer(this)), _shouldResetSelection(true) {
  setDefaultActionShortcuts({Keyboard::Shortcut::enter(), Keyboard::Shortcut::submit()});
  m_selector->setMinimumWidth(300);
  m_selector->setEnableDefaultFilter(false);
  m_selector->setFocusPolicy(Qt::NoFocus);
  m_split->setMainWidget(m_list);
  m_split->setDetailWidget(m_detail);
  m_split->detailWidget()->hide();

  m_content->addWidget(m_emptyView);
  m_content->addWidget(m_split);
  m_content->setCurrentWidget(m_split);
  setupUI(m_content);

  _debounce->setSingleShot(true);
  connect(_debounce, &QTimer::timeout, this, &ExtensionListComponent::handleDebouncedSearchNotification);
  connect(m_list, &ExtensionList::selectionChanged, this, &ExtensionListComponent::onSelectionChanged);
  connect(m_list, &ExtensionList::itemActivated, this, &ExtensionListComponent::onItemActivated);
  connect(m_selector, &SelectorInput::selectionChanged, this,
          &ExtensionListComponent::handleDropdownSelectionChanged);
  connect(m_selector, &SelectorInput::textChanged, this,
          &ExtensionListComponent::handleDropdownSearchChanged);
}

ExtensionListComponent::~ExtensionListComponent() {}
