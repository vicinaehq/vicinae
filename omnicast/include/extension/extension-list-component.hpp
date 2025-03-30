#pragma once
#include "app.hpp"
#include "extend/list-model.hpp"
#include "extension/extension-component.hpp"
#include "omni-icon.hpp"
#include "ui/omni-list.hpp"
#include <QJsonArray>
#include <qboxlayout.h>
#include <qnamespace.h>

class ExtensionListItem : public AbstractDefaultListItem {
  ListItemViewModel _item;

  ItemData data() const override {
    return {
        .iconUrl = OmniIconUrl(_item.icon),
        .name = _item.title,
        .category = _item.subtitle,
    };
  }

  QString id() const override { return _item.id; }

public:
  const ListItemViewModel &model() const { return _item; }

  ExtensionListItem(const ListItemViewModel &model) : _item(model) {}
};

class BuiltinExtensionItemFilter : public OmniList::AbstractItemFilter {
  QString _filterText;

  bool matches(const OmniList::AbstractVirtualItem &base) override {
    auto item = static_cast<const ExtensionListItem &>(base);

    return item.model().title.contains(_filterText, Qt::CaseInsensitive);
  }

public:
  BuiltinExtensionItemFilter(const QString &filterText) : _filterText(filterText) {}
};

class ExtensionListComponent : public AbstractExtensionRootComponent {
  ListModel _model;
  QVBoxLayout *_layout;
  OmniList *_list;

public:
  void render(const RenderModel &baseModel) override {
    _model = std::get<ListModel>(baseModel);

    if (!_model.navigationTitle.isEmpty()) { setNavigationTitle(_model.navigationTitle); }
    if (!_model.searchPlaceholderText.isEmpty()) { setSearchPlaceholderText(_model.searchPlaceholderText); }

    setLoading(_model.isLoading);

    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

    items.reserve(_model.items.size());

    qDebug() << "render items from list model" << _model.items.size();

    for (const auto &item : _model.items) {
      if (auto listItem = std::get_if<ListItemViewModel>(&item)) {
        items.push_back(std::make_unique<ExtensionListItem>(*listItem));
      } else if (auto section = std::get_if<ListSectionModel>(&item)) {
        items.push_back(std::make_unique<OmniList::VirtualSection>(section->title));

        for (const auto &item : section->children) {
          items.push_back(std::make_unique<ExtensionListItem>(item));
        }
      }
    }

    _list->updateFromList(items, OmniList::SelectFirst);
  }

  void onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                          const OmniList::AbstractVirtualItem *previous) {
    if (!next) return;

    auto item = static_cast<const ExtensionListItem *>(next);

    if (auto pannel = item->model().actionPannel) { emit updateActionPannel(*pannel); }

    if (auto handler = _model.onSelectionChanged; !handler.isEmpty()) {
      emit notifyEvent(handler, {next->id()});
    }
  }

  void onSearchChanged(const QString &text) override {
    if (_model.isFiltering || _model.onSearchTextChange.isEmpty()) {
      _list->setFilter(std::make_unique<BuiltinExtensionItemFilter>(text));
    }

    if (auto handler = _model.onSearchTextChange; !handler.isEmpty()) { emit notifyEvent(handler, {text}); }
  }

  ExtensionListComponent(AppWindow &app)
      : AbstractExtensionRootComponent(app), _layout(new QVBoxLayout), _list(new OmniList) {
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    _layout->addWidget(_list);
    setLayout(_layout);

    connect(_list, &OmniList::selectionChanged, this, &ExtensionListComponent::onSelectionChanged);
  }
};
