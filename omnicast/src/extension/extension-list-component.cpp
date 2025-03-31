#include "extension/extension-list-component.hpp"
#include "app.hpp"

void ExtensionListComponent::render(const RenderModel &baseModel) {
  auto newModel = std::get<ListModel>(baseModel);

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

  std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

  items.reserve(newModel.items.size());

  qDebug() << "render items from list model" << newModel.items.size();

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

  if (_shouldResetSelection) {
    _shouldResetSelection = false;
    _list->updateFromList(items, OmniList::SelectFirst);
  } else {
    _list->updateFromList(items, OmniList::KeepSelection);
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

  if (auto &pannel = item->model().actionPannel) { emit updateActionPannel(*pannel); }
  if (auto handler = _model.onSelectionChanged) { emit notifyEvent(*handler, {next->id()}); }
}

void ExtensionListComponent::handleDebouncedSearchNotification() {
  auto text = searchText();

  if (_model.filtering) { _list->setFilter(std::make_unique<BuiltinExtensionItemFilter>(text)); }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;

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
