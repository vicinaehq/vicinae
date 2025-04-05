#include "extension/extension-list-component.hpp"
#include "app.hpp"
#include "ui/action-pannel/action-item.hpp"

KeyboardShortcutModel primaryShortcut{.key = "return"};
KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

void ExtensionListComponent::render(const RenderModel &baseModel) {
  auto newModel = std::get<ListModel>(baseModel);

  if (!newModel.navigationTitle.isEmpty()) { setNavigationTitle(newModel.navigationTitle); }
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

  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

  items.reserve(newModel.items.size());

  for (const auto &item : newModel.items) {
    if (auto listItem = std::get_if<ListItemViewModel>(&item)) {
      items.push_back(std::make_unique<ExtensionListItem>(*listItem));
    } else if (auto section = std::get_if<ListSectionModel>(&item)) {
      items.push_back(std::make_unique<OmniList::VirtualSection>(section->title));

      for (const auto &item : section->children) {
        items.push_back(std::make_unique<ExtensionListItem>(item));
      }
    }
  }

  if (!newModel.searchText) {
    if (newModel.filtering) {
      _list->setFilter(std::make_unique<BuiltinExtensionItemFilter>(searchText()));
    } else {
      _list->clearFilter();
    }
  }

  _list->invalidateCache();

  if (_shouldResetSelection) {
    _shouldResetSelection = false;
    _list->updateFromList(items, OmniList::SelectFirst);
  } else {
    _list->updateFromList(items, OmniList::KeepSelection);
  }

  if (_list->isShowingEmptyState()) {
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
    QString oldSelectedId = _list->selected() ? _list->selected()->id() : "";

    if (_list->selected() && _list->selected()->isListItem() && _list->selected()->id() == oldSelectedId) {
      onSelectionChanged(_list->selected(), nullptr);
    }
  }

  _model = newModel;
}

void ExtensionListComponent::onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                                                const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    if (auto &pannel = _model.actions) { emit updateActionPannel(*pannel); }

    return;
  }

  qDebug() << "selection" << next->id();

  auto item = static_cast<const ExtensionListItem *>(next);
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

    emit notifyEvent(*handler, {text});
  }
}

void ExtensionListComponent::onSearchChanged(const QString &text) { _debounce->start(); }

ExtensionListComponent::ExtensionListComponent(AppWindow &app)
    : AbstractExtensionRootComponent(app), _debounce(new QTimer(this)), _layout(new QVBoxLayout),
      _list(new OmniList), _shouldResetSelection(true) {
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(0);
  _layout->addWidget(_list);
  setLayout(_layout);

  _debounce->setSingleShot(true);
  connect(_debounce, &QTimer::timeout, this, &ExtensionListComponent::handleDebouncedSearchNotification);
  connect(_list, &OmniList::selectionChanged, this, &ExtensionListComponent::onSelectionChanged);
}
