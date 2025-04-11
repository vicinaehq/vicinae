#include "extension/extension-grid-component.hpp"
#include "app.hpp"
#include "extension/extension-list-component.hpp"
#include "ui/omni-grid.hpp"

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);
static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

bool ExtensionGridComponent::inputFilter(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Left:
  case Qt::Key_Right:
  case Qt::Key_Up:
  case Qt::Key_Down:
  case Qt::Key_Return:
    QApplication::sendEvent(_grid, event);
    return true;
  }

  return AbstractExtensionRootComponent::inputFilter(event);
}

void ExtensionGridComponent::render(const RenderModel &baseModel) {
  auto newModel = std::get<GridModel>(baseModel);

  if (!newModel.navigationTitle.isEmpty()) { setNavigationTitle(newModel.navigationTitle); }
  if (!newModel.searchPlaceholderText.isEmpty()) { setSearchPlaceholderText(newModel.searchPlaceholderText); }

  _grid->setColumns(newModel.columns);
  _grid->setInset(newModel.inset);

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

  std::vector<std::unique_ptr<OmniGrid::AbstractVirtualItem>> items;

  items.reserve(newModel.items.size());

  for (const auto &item : newModel.items) {
    if (auto listItem = std::get_if<GridItemViewModel>(&item)) {
      items.push_back(std::make_unique<ExtensionGridItem>(*listItem));
    } else if (auto section = std::get_if<GridSectionModel>(&item)) {
      items.push_back(std::make_unique<OmniGrid::GridSection>(section->title, section->columns, 10));

      for (const auto &item : section->children) {
        auto extensionItem = std::make_unique<ExtensionGridItem>(item);

        extensionItem->setInset(section->inset);
        items.push_back(std::move(extensionItem));
      }
    }
  }

  if (!newModel.searchText) {
    if (newModel.filtering) {
      _grid->setFilter(std::make_unique<BuiltinExtensionGridItemFilter>(searchText()));
    } else {
      _grid->clearFilter();
    }
  }

  _grid->invalidateCache();

  if (_shouldResetSelection) {
    _shouldResetSelection = false;
    _grid->updateFromList(items, OmniGrid::SelectFirst);
  } else {
    _grid->updateFromList(items, OmniGrid::KeepSelection);
  }

  if (_grid->isShowingEmptyState()) {
    size_t i = 0;

    if (auto pannel = newModel.actions) {
      for (auto &item : pannel->children) {
        if (auto action = std::get_if<ActionModel>(&item)) {
          if (i == 0) action->shortcut = primaryShortcut;
          if (i == 1) action->shortcut = secondaryShortcut;

          ++i;
        }
      }

      emit updateActionPannel(*pannel);
    }
  } else {
    QString oldSelectedId = _grid->selected() ? _grid->selected()->id() : "";

    if (_grid->selected() && _grid->selected()->isListItem() && _grid->selected()->id() == oldSelectedId) {
      onSelectionChanged(_grid->selected(), nullptr);
    }
  }

  _model = newModel;
}

void ExtensionGridComponent::onSelectionChanged(const OmniGrid::AbstractVirtualItem *next,
                                                const OmniGrid::AbstractVirtualItem *previous) {
  if (!next) {
    if (auto &pannel = _model.actions) { emit updateActionPannel(*pannel); }

    return;
  }

  qDebug() << "selection" << next->id();

  auto item = static_cast<const ExtensionGridItem *>(next);
  size_t i = 0;

  if (auto pannel = item->model().actionPannel) {
    for (auto &item : pannel->children) {
      if (auto action = std::get_if<ActionModel>(&item)) {
        if (i == 0) action->shortcut = primaryShortcut;
        if (i == 1) action->shortcut = secondaryShortcut;

        ++i;
      }
    }

    emit updateActionPannel(*pannel);
  }
  if (auto handler = _model.onSelectionChanged) { emit notifyEvent(*handler, {next->id()}); }
}

void ExtensionGridComponent::handleDebouncedSearchNotification() {
  auto text = searchText();

  if (_model.filtering) {
    _grid->setFilter(std::make_unique<BuiltinExtensionItemFilter>(text));
  } else {
    _grid->clearFilter();
  }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;

    qDebug() << "[DEBUG] sending search changed event" << text;

    emit notifyEvent(*handler, {text});
  }
}

void ExtensionGridComponent::onItemActivated(const OmniGrid::AbstractVirtualItem &item) {
  selectPrimaryAction();
}

void ExtensionGridComponent::onSearchChanged(const QString &text) { _debounce->start(); }

ExtensionGridComponent::ExtensionGridComponent(AppWindow &app)
    : AbstractExtensionRootComponent(app), _debounce(new QTimer(this)), _layout(new QVBoxLayout),
      _grid(new OmniGrid), _shouldResetSelection(true) {
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(0);
  _layout->addWidget(_grid);
  setLayout(_layout);

  _debounce->setSingleShot(true);
  connect(_debounce, &QTimer::timeout, this, &ExtensionGridComponent::handleDebouncedSearchNotification);
  connect(_grid, &OmniGrid::selectionChanged, this, &ExtensionGridComponent::onSelectionChanged);
  connect(_grid, &OmniGrid::itemActivated, this, &ExtensionGridComponent::onItemActivated);
}
