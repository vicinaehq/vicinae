#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "extend/grid-model.hpp"
#include <qdebug.h>
#include "extension/extension-component.hpp"
#include "omni-icon.hpp"
#include "ui/omni-grid.hpp"
#include "ui/omni-list.hpp"
#include <QJsonArray>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qtimer.h>

class ExtensionGridItem : public OmniGrid::AbstractGridItem {
  GridItemViewModel _item;

  QString id() const override { return _item.id; }

  QWidget *centerWidget() const override {
    auto icon = new OmniIcon;

    icon->setFixedSize(32, 32);
    icon->setUrl(_item.content);

    return icon;
  }

  const QString &name() const { return _item.title; }

public:
  const GridItemViewModel &model() const { return _item; }

  ExtensionGridItem(const GridItemViewModel &model) : _item(model) {}
};

class BuiltinExtensionGridItemFilter : public OmniList::AbstractItemFilter {
  QString _filterText;

  bool matches(const OmniList::AbstractVirtualItem &base) override {
    auto item = static_cast<const ExtensionGridItem &>(base);

    if (item.model().title.contains(_filterText, Qt::CaseInsensitive)) return true;

    if (auto keywords = item.model().keywords) {
      for (const auto &keyword : *keywords) {
        if (keyword.contains(_filterText, Qt::CaseInsensitive)) return true;
      }
    }

    return false;
  }

public:
  BuiltinExtensionGridItemFilter(const QString &filterText) : _filterText(filterText) {}
};

class ExtensionGridComponent : public AbstractExtensionRootComponent {
  GridModel _model;
  QVBoxLayout *_layout;
  OmniGrid *_grid;
  bool _shouldResetSelection;
  QTimer *_debounce;

public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                          const OmniList::AbstractVirtualItem *previous);
  void onItemActivated(const OmniList::AbstractVirtualItem &item);
  void handleDebouncedSearchNotification();
  void onSearchChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;

  ExtensionGridComponent(AppWindow &app);
};
