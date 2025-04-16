#include "extension/extension-list-component.hpp"
#include "app.hpp"
#include "extension/extension-list-detail.hpp"
#include "ui/typography.hpp"
#include <qnamespace.h>

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

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

  return AbstractExtensionRootComponent::inputFilter(event);
}

void ExtensionListComponent::render(const RenderModel &baseModel) {
  qDebug() << "render";
  auto newModel = std::get<ListModel>(baseModel);

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

  _model = newModel;

  if (_shouldResetSelection) {
    _shouldResetSelection = false;
    _list->updateFromList(items, OmniList::SelectFirst);
  } else {
    _list->updateFromList(items, OmniList::KeepSelection);
  }

  if (!_list->isShowingEmptyState()) { m_split->setDetailVisibility(newModel.isShowingDetail); }

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
  }
}

void ExtensionListComponent::onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                                                const OmniList::AbstractVirtualItem *previous) {
  if (!next) {
    m_split->setDetailVisibility(false);

    if (auto &pannel = _model.actions) { emit updateActionPannel(*pannel); }

    return;
  }

  m_split->setDetailVisibility(_model.isShowingDetail);

  qDebug() << "set visibility of" << next->id() << _model.isShowingDetail;

  auto item = static_cast<const ExtensionListItem *>(next);
  size_t i = 0;

  if (auto detail = item->model().detail) {
    qDebug() << "set markdown for" << next->id();
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

void ExtensionListComponent::onItemActivated(const OmniList::AbstractVirtualItem &item) {
  selectPrimaryAction();
}

void ExtensionListComponent::onSearchChanged(const QString &text) { _debounce->start(); }

ExtensionListComponent::ExtensionListComponent(AppWindow &app)
    : AbstractExtensionRootComponent(app), _debounce(new QTimer(this)), _layout(new QVBoxLayout),
      _list(new OmniList), _shouldResetSelection(true) {

  m_split->setMainWidget(_list);
  m_split->setDetailWidget(m_detail);

  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(0);
  _layout->addWidget(m_split);
  setLayout(_layout);

  _debounce->setSingleShot(true);
  connect(_debounce, &QTimer::timeout, this, &ExtensionListComponent::handleDebouncedSearchNotification);
  connect(_list, &OmniList::selectionChanged, this, &ExtensionListComponent::onSelectionChanged);
  connect(_list, &OmniList::itemActivated, this, &ExtensionListComponent::onItemActivated);
}
