#include "extension/extension-grid-component.hpp"
#include "extend/grid-model.hpp"
#include "extension/extension-view.hpp"
#include "ui/omni-grid/grid-item-content-widget.hpp"
#include "ui/omni-list/omni-list.hpp"

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);
static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

bool ExtensionGridComponent::inputFilter(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier) {
    switch (event->key()) {
    case Qt::Key_H:
      return m_list->selectLeft();
    case Qt::Key_L:
      return m_list->selectRight();
    case Qt::Key_K:
      return m_list->selectUp();
    case Qt::Key_J:
      return m_list->selectDown();
    }
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
  m_list->setInset(newModel.inset.value_or(GridItemContentWidget::Inset::Small));
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
    if (auto panel = selected->actionPannel) { setActionPanel(*panel); }
  }

  if (m_list->empty()) {
    if (auto pannel = newModel.actions) { setActionPanel(*pannel); }
  }
}

void ExtensionGridComponent::onSelectionChanged(const GridItemViewModel *next) {
  if (!next) {
    if (auto &pannel = _model.actions) { setActionPanel(*pannel); }
    return;
  }

  if (auto &panel = next->actionPannel) { setActionPanel(*panel); }
  if (auto handler = _model.onSelectionChanged) { notify(*handler, {next->id}); }
}

void ExtensionGridComponent::handleDebouncedSearchNotification() { auto text = searchText(); }

void ExtensionGridComponent::onItemActivated(const GridItemViewModel &item) { executePrimaryAction(); }

void ExtensionGridComponent::textChanged(const QString &text) {
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
}

ExtensionGridComponent::ExtensionGridComponent() : _debounce(new QTimer(this)), _shouldResetSelection(true) {
  setDefaultActionShortcuts({primaryShortcut, secondaryShortcut});
  _debounce->setSingleShot(true);
  setupUI(m_list);
  connect(_debounce, &QTimer::timeout, this, &ExtensionGridComponent::handleDebouncedSearchNotification);
  connect(m_list, &ExtensionGridList::selectionChanged, this, &ExtensionGridComponent::onSelectionChanged);
  connect(m_list, &ExtensionGridList::itemActivated, this, &ExtensionGridComponent::onItemActivated);
}
