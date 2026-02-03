#include "extension/extension-grid-component.hpp"
#include "extend/grid-model.hpp"
#include "extend/dropdown-model.hpp"
#include "extension/extension-view.hpp"
#include "extension/form/extension-dropdown.hpp"
#include "keyboard/keybind-manager.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/form/selector-input.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "service-registry.hpp"
#include "config/config.hpp"
#include <chrono>
#include <memory>

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);
static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

void ExtensionGridComponent::renderDropdown(const DropdownModel &dropdown) {
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
          m_selector->addSection("", freeSectionItems);
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

bool ExtensionGridComponent::inputFilter(QKeyEvent *event) {
  auto config = ServiceRegistry::instance()->config();
  const std::string &keybinding = config->value().keybinding;

  if (event->modifiers() == Qt::ControlModifier) {
    if (KeyBindingService::isLeftKey(event, keybinding)) { return m_list->selectLeft(); }
    if (KeyBindingService::isRightKey(event, keybinding)) { return m_list->selectRight(); }
    if (KeyBindingService::isUpKey(event, keybinding)) { return m_list->selectUp(); }
    if (KeyBindingService::isDownKey(event, keybinding)) { return m_list->selectDown(); }
  }

  if (event->modifiers().toInt() == 0) {
    switch (event->key()) {
    case Qt::Key_Left:
      return m_list->selectLeft();
    case Qt::Key_Right:
      return m_list->selectRight();
    case Qt::Key_Up:
      return m_list->selectUp();
    case Qt::Key_Down:
      return m_list->selectDown();
    case Qt::Key_Tab:
      if (!context()->navigation->hasCompleter()) {
        m_list->selectNext();
        return true;
      }
      break;
    case Qt::Key_Return:
      m_list->activateCurrentSelection();
      return true;
    }
  }

  if (m_selector->isVisible() &&
      KeybindManager::instance()->resolve(Keybind::OpenSearchAccessorySelector) == event) {
    m_selector->openSelector();
    return true;
  }

  return ExtensionSimpleView::inputFilter(event);
}

void ExtensionGridComponent::render(const RenderModel &baseModel) {
  auto newModel = std::get<GridModel>(baseModel);

  if (auto accessory = newModel.searchBarAccessory) {
    auto dropdown = std::get<DropdownModel>(*accessory);

    renderDropdown(dropdown);
  }

  setSearchAccessoryVisiblity(newModel.searchBarAccessory.has_value() && isVisible());

  if (!newModel.navigationTitle.empty()) { setNavigationTitle(newModel.navigationTitle.c_str()); }
  if (!newModel.searchPlaceholderText.empty()) {
    setSearchPlaceholderText(newModel.searchPlaceholderText.c_str());
  }

  if (auto text = newModel.searchText) { setSearchText(text->c_str()); }

  if (newModel.throttle != _model.throttle) {
    _debounce->stop();

    if (newModel.throttle) {
      _debounce->setInterval(THROTTLE_DEBOUNCE_DURATION);
    } else {
      _debounce->setInterval(0);
    }
  }

  setLoading(newModel.isLoading);

  // if (newModel.dirty) {
  OmniList::SelectionPolicy policy = OmniList::SelectFirst;

  if (_shouldResetSelection) {
    _shouldResetSelection = false;
    policy = OmniList::SelectFirst;
  } else {
    policy = OmniList::PreserveSelection;
  }

  m_list->setColumns(newModel.columns.value_or(1));
  m_list->setInset(newModel.inset);
  m_list->setAspectRatio(newModel.aspectRatio);
  m_list->setFit(newModel.fit);
  m_list->setModel(newModel.items, policy);

  if (!newModel.searchText) {
    if (_shouldResetSelection) {
      if (newModel.filtering) {
        m_list->setFilter(searchText());
      } else {
        m_list->setFilter("");
      }
    }
  }

  _model = newModel;

  if (auto selected = m_list->selected()) {
    if (auto panel = selected->actionPannel) {
      setActionPanel(*panel);
    } else {
      clearActions();
    }
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
      m_content->setCurrentWidget(m_list);
    }
  } else {
    m_content->setCurrentWidget(m_list);
  }
}

void ExtensionGridComponent::onSelectionChanged(const GridItemViewModel *next) {
  if (!next) {
    if (auto &pannel = _model.actions) {
      setActionPanel(*pannel);
    } else {
      clearActions();
    }
    return;
  }

  if (auto &panel = next->actionPannel) {
    setActionPanel(*panel);
  } else {
    clearActions();
  }
  if (auto handler = _model.onSelectionChanged) { notify(handler->c_str(), {next->id.c_str()}); }
}

void ExtensionGridComponent::handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item) {
  if (auto accessory = _model.searchBarAccessory) {
    if (auto dropdown = std::get_if<DropdownModel>(&*accessory)) {
      if (auto onChange = dropdown->onChange) { notify(*onChange, {item.generateId()}); }
    }
  }
}

void ExtensionGridComponent::handleDropdownSearchChanged(const QString &text) {
  if (auto accessory = _model.searchBarAccessory) {
    if (auto dropdown = std::get_if<DropdownModel>(&*accessory)) {
      m_dropdownShouldResetSelection = !dropdown->filtering.enabled;

      if (auto onChange = dropdown->onSearchTextChange) { emit notify(*onChange, {text}); }
    }
  }
}

void ExtensionGridComponent::handleDebouncedSearchNotification() {
  auto text = searchText();

  if (_model.filtering) {
    m_list->setFilter(text);
  } else {
    m_list->setFilter("");
  }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;

    notify(handler->c_str(), {text});
  }

  if (m_list->empty()) {
    if (_model.emptyView) { m_content->setCurrentWidget(m_emptyView); }
  } else {
    m_content->setCurrentWidget(m_list);
  }
}

void ExtensionGridComponent::onItemActivated(const GridItemViewModel &item) { executePrimaryAction(); }

void ExtensionGridComponent::textChanged(const QString &text) {
  if (_model.throttle) {
    _debounce->start();
  } else {
    handleDebouncedSearchNotification();
  }
}

void ExtensionGridComponent::initialize() { setSearchAccessoryVisiblity(false); }

ExtensionGridComponent::ExtensionGridComponent() : _debounce(new QTimer(this)), _shouldResetSelection(true) {
  setDefaultActionShortcuts({Keyboard::Shortcut::enter(), Keyboard::Shortcut::submit()});
  m_selector->setMinimumWidth(300);
  m_selector->setEnableDefaultFilter(false);
  m_selector->setFocusPolicy(Qt::NoFocus);
  _debounce->setSingleShot(true);
  m_content->addWidget(m_list);
  m_content->addWidget(m_emptyView);
  m_content->setCurrentWidget(m_list);
  setupUI(m_content);
  connect(_debounce, &QTimer::timeout, this, &ExtensionGridComponent::handleDebouncedSearchNotification);
  connect(m_list, &ExtensionGridList::selectionChanged, this, &ExtensionGridComponent::onSelectionChanged);
  connect(m_list, &ExtensionGridList::itemActivated, this, &ExtensionGridComponent::onItemActivated);
  connect(m_selector, &SelectorInput::selectionChanged, this,
          &ExtensionGridComponent::handleDropdownSelectionChanged);
  connect(m_selector, &SelectorInput::textChanged, this,
          &ExtensionGridComponent::handleDropdownSearchChanged);
}
