#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/ellided-label.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include "ui/tooltip.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

struct ColoredAccessoryText {
  ColorLike color;
  QString text;
  bool fillBackground;
};

struct ListAccessory {
  QString text;
  std::optional<ColorLike> color;
  QString tooltip;
  bool fillBackground;
  std::optional<OmniIconUrl> icon;
};

class AccessoryWidget : public QWidget {
  QHBoxLayout *_layout;
  OmniIcon *_icon = nullptr;
  EllidedLabel *_label = nullptr;
  Tooltip *_tooltip;
  ListAccessory _accessory;

  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    ColorLike colorLike = _accessory.color ? (*_accessory.color) : theme.colors.subtext;
    QPainter painter(this);

    if (_accessory.fillBackground) {
      int cornerRadius = width() / 4;

      if (auto color = std::get_if<QColor>(&colorLike)) {
        color->setAlphaF(0.2);
        painter.setBrush(*color);
        painter.drawRoundedRect(rect(), cornerRadius, cornerRadius);
      } else if (auto lgrad = std::get_if<ThemeLinearGradient>(&colorLike)) {
        QLinearGradient gradient;

        for (int i = 0; i != lgrad->points.size(); ++i) {
          auto point = lgrad->points[i];

          point.setAlphaF(0.2);
          gradient.setColorAt(i, point);
        }

        painter.setBrush(gradient);
        painter.drawRoundedRect(rect(), cornerRadius, cornerRadius);
      } else if (auto rgrad = std::get_if<ThemeRadialGradient>(&colorLike)) {
        QRadialGradient gradient(rect().center(), rect().width() / 2.0);

        gradient.setSpread(QGradient::PadSpread);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

        for (int i = 0; i != rgrad->points.size(); ++i) {
          auto point = rgrad->points[i];

          point.setAlphaF(0.2);
          gradient.setColorAt(i, point);
        }

        painter.setBrush(gradient);
        painter.drawRoundedRect(rect(), cornerRadius, cornerRadius);
      }
    }

    QWidget::paintEvent(event);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    _icon->setFixedSize(event->size().height(), event->size().height());
  }

public:
  AccessoryWidget(QWidget *parent = nullptr)
      : QWidget(parent), _layout(new QHBoxLayout), _icon(new OmniIcon), _label(new EllidedLabel),
        _tooltip(new Tooltip) {
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setAlignment(Qt::AlignVCenter);
    _layout->addWidget(_icon, 0, Qt::AlignVCenter);
    _layout->addWidget(_label, 0, Qt::AlignVCenter);

    setLayout(_layout);
  }

  void setAccessory(const ListAccessory &accessory) {
    if (accessory.icon) { _icon->setUrl(*accessory.icon); }

    _icon->setVisible(accessory.icon.has_value());
    _label->setText(accessory.text);
    _label->setVisible(!accessory.text.isEmpty());

    _accessory = accessory;
  }
};

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
      AccessoryWidget *widget = nullptr;

      if (_layout->count() > i) {
        widget = static_cast<AccessoryWidget *>(_layout->itemAt(i)->widget());
        widget->setAccessory(accessory);
        widget->show();
      } else {
        widget = new AccessoryWidget();
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
  OmniIcon *_icon;
  QLabel *_name;
  QLabel *_category;
  AccessoryListWidget *_accessoryList;

public:
  void setAccessories(const AccessoryList &list);
  void setName(const QString &name);
  void setCategory(const QString &category);
  void setIconUrl(const OmniIconUrl &url);

  DefaultListItemWidget(const OmniIconUrl &iconUrl, const QString &name, const QString &category,
                        const AccessoryList &accessories, QWidget *parent = nullptr);
};
