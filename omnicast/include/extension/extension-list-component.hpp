#pragma once
#include "app.hpp"
#include "builtin_icon.hpp"
#include "extend/list-model.hpp"
#include "extension/extension-component.hpp"
#include "omni-icon.hpp"
#include "ui/omni-list.hpp"
#include <QJsonArray>
#include <chrono>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qresource.h>
#include <qtimer.h>

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

class ExtensionOmniIconUrl : public OmniIconUrl {
public:
  ExtensionOmniIconUrl(const ImageLikeModel &imageLike) {
    if (auto image = std::get_if<ExtensionImageModel>(&imageLike)) {
      if (QFile(":icons/" + image->source).exists()) {
        setType(OmniIconType::Builtin);
        setName(image->source);
      }

      if (QFile(image->source).exists()) {
        setType(OmniIconType::Local);
        setName(image->source);
      }

      QUrl url(image->source);

      if (url.isValid()) {
        if (url.scheme() == "file") {
          setType(OmniIconType::Local);
          setName(url.host() + url.path());
          return;
        }

        if (url.scheme() == "https" || url.scheme() == "http") {
          setType(OmniIconType::Http);
          setName(url.host() + url.path());
        }
      }
    }
  }
};

class ExtensionListItem : public AbstractDefaultListItem {
  ListItemViewModel _item;

  ItemData data() const override {
    return {
        .iconUrl = _item.icon,
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
  bool _shouldResetSelection;
  QTimer *_debounce;

public:
  void render(const RenderModel &baseModel) override;
  void onSelectionChanged(const OmniList::AbstractVirtualItem *next,
                          const OmniList::AbstractVirtualItem *previous);
  void handleDebouncedSearchNotification();
  void onSearchChanged(const QString &text) override;

  ExtensionListComponent(AppWindow &app);
};
