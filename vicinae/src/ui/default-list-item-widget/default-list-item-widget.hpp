#pragma once
#include "../image/url.hpp"
#include "ui/typography/typography.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/selectable-omni-list-widget/selectable-omni-list-widget.hpp"
#include <qboxlayout.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qwidget.h>
#include "ui/list-accessory/list-accessory.hpp"

class AccessoryListWidget : public QWidget {
  QHBoxLayout *_layout;

public:
  AccessoryListWidget(QWidget *parent = nullptr) : QWidget(parent), _layout(new QHBoxLayout) {
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(15);
    setLayout(_layout);
  }

  void setAccessories(const std::vector<ListAccessory> &accessories) {
    for (int i = 0; i != accessories.size(); ++i) {
      auto &accessory = accessories[i];
      ListAccessoryWidget *widget = nullptr;

      if (_layout->count() > i) {
        widget = static_cast<ListAccessoryWidget *>(_layout->itemAt(i)->widget());
        widget->setAccessory(accessory);
        widget->show();
      } else {
        widget = new ListAccessoryWidget();
        widget->setAccessory(accessory);
        _layout->addWidget(widget);
      }
    }

    // keep old widgets, as we might be able to reuse them later
    for (int i = accessories.size(); i < _layout->count(); ++i) {
      _layout->itemAt(i)->widget()->hide();
    }
  }
};

using AccessoryList = std::vector<ListAccessory>;

class DefaultListItemWidget : public SelectableOmniListWidget {
  Omnimg::ImageWidget *_icon = new Omnimg::ImageWidget(this);
  TypographyWidget *_name = new TypographyWidget(this);
  TypographyWidget *_category = new TypographyWidget(this);
  AccessoryListWidget *_accessoryList = new AccessoryListWidget(this);
  ListAccessoryWidget *m_alias = new ListAccessoryWidget(this);

public:
  void setAccessories(const AccessoryList &list);
  void setName(const QString &name);
  void setSubtitle(const std::variant<QString, std::filesystem::path> &subtitle);
  void setIconUrl(const std::optional<ImageURL> &url);
  void setAlias(const QString &title);

  DefaultListItemWidget(QWidget *parent = nullptr);
};
