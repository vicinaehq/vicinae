#include "extension/extension-grid-component.hpp"
#include "extend/grid-model.hpp"
#include "extension/extension-view.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "services/config/config-service.hpp"
#include "service-registry.hpp"

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);
static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

bool ExtensionGridComponent::inputFilter(QKeyEvent *event) {
  auto config = ServiceRegistry::instance()->config();
  const QString &keybinding = config->value().keybinding;

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
    case Qt::Key_Home:
      return m_list->selectHome();
    case Qt::Key_End:
      return m_list->selectEnd();
    case Qt::Key_Return:
      m_list->activateCurrentSelection();
      return true;
    }
  }

  return ExtensionSimpleView::inputFilter(event);
}

void ExtensionGridComponent::render(const RenderModel &baseModel) {
  auto newModel = std::get<GridModel>(baseModel);

  if (auto accessory = newModel.searchBarAccessory) {
    // auto dropdown = std::get<DropdownModel>(*accessory);

    // renderDropdown(dropdown);
  }

  // m_selector->setVisible(newModel.searchBarAccessory.has_value() && isVisible());

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
  if (auto handler = _model.onSelectionChanged) { notify(*handler, {next->id}); }
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

    notify(*handler, {text});
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

ExtensionGridComponent::ExtensionGridComponent() : _debounce(new QTimer(this)), _shouldResetSelection(true) {
  setDefaultActionShortcuts({Keyboard::Shortcut::enter(), Keyboard::Shortcut::submit()});
  _debounce->setSingleShot(true);
  m_content->addWidget(m_list);
  m_content->addWidget(m_emptyView);
  m_content->setCurrentWidget(m_list);
  setupUI(m_content);
  connect(_debounce, &QTimer::timeout, this, &ExtensionGridComponent::handleDebouncedSearchNotification);
  connect(m_list, &ExtensionGridList::selectionChanged, this, &ExtensionGridComponent::onSelectionChanged);
  connect(m_list, &ExtensionGridList::itemActivated, this, &ExtensionGridComponent::onItemActivated);
}
