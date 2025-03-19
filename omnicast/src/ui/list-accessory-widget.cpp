#include "ui/list-accessory-widget.hpp"

void ListAccessoryWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);

  if (_accessory.fillBackground && _accessory.color) {
    int cornerRadius = 6;

    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), *_accessory.color, 6, 0.2);
  }

  QWidget::paintEvent(event);
}

void ListAccessoryWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  auto geo = rect().marginsRemoved(_layout->contentsMargins());
  auto size = geo.size();

  _icon->setFixedSize(size.height(), size.height());
}

void ListAccessoryWidget::setAccessory(const ListAccessory &accessory) {
  if (accessory.icon) {
    auto url = *accessory.icon;

    url.setFill(accessory.color);
    _icon->setUrl(url);
  }

  _icon->setVisible(accessory.icon.has_value());
  _label->setText(accessory.text);
  _label->setVisible(!accessory.text.isEmpty());
  _accessory = accessory;
}

ListAccessoryWidget::ListAccessoryWidget(QWidget *parent)
    : QWidget(parent), _layout(new QHBoxLayout), _icon(new OmniIcon), _label(new EllidedLabel),
      _tooltip(new Tooltip) {
  _layout->setContentsMargins(5, 3, 5, 3);
  _layout->setAlignment(Qt::AlignVCenter);
  _layout->addWidget(_icon);
  _layout->addWidget(_label);

  setLayout(_layout);
}

ListAccessoryWidget::~ListAccessoryWidget() { _tooltip->deleteLater(); }
