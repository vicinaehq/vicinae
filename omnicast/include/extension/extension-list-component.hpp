#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "extend/list-model.hpp"
#include <qdebug.h>
#include "extension/extension-component.hpp"
#include "extension/extension-list-detail.hpp"
#include "ui/omni-list.hpp"
#include "ui/split-detail.hpp"
#include <QJsonArray>
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qtimer.h>

class ExtensionListItem : public AbstractDefaultListItem {
  ListItemViewModel _item;

  ItemData data() const override {
    return {
        .iconUrl = _item.icon,
        .name = _item.title,
        .category = _item.subtitle,
    };
  }

  bool hasPartialUpdates() const override { return true; }

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
  SplitDetailWidget *m_split = new SplitDetailWidget;
  ExtensionListDetail *m_detail = new ExtensionListDetail;
  ListModel _model;
  QVBoxLayout *_layout;
  OmniList *_list;
  bool _shouldResetSelection;
  QTimer *_debounce;
  int m_renderCount = 0;

  void resizeEvent(QResizeEvent *event) override {
    AbstractExtensionRootComponent::resizeEvent(event);
    qDebug() << "extension list component resize" << event->size();
  }

public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                          const OmniList::AbstractVirtualItem *previous);
  void onItemActivated(const OmniList::AbstractVirtualItem &item);
  void handleDebouncedSearchNotification();
  void onSearchChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;

  ExtensionListComponent(AppWindow &app);
};
