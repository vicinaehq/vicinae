#include "list-accessory.hpp"
#include "theme.hpp"
#include "ui/image/image.hpp"
#include "ui/image/url.hpp"
#include "ui/typography/typography.hpp"

void ListAccessoryWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);

  if (_accessory.fillBackground && _accessory.color) {
    int cornerRadius = 4;

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

  //_icon->setFixedSize(size.height(), size.height());
}

void ListAccessoryWidget::setAccessory(const ListAccessory &accessory) {
  if (accessory.icon) {
    auto url = *accessory.icon;

    if (accessory.color && url.type() == ImageURLType::Builtin) { url.setFill(accessory.color.value()); }

    _icon->setUrl(url);
  }

  _icon->setVisible(accessory.icon.has_value());
  _text->setText(accessory.text);
  _text->setVisible(!accessory.text.isEmpty());
  if (accessory.color) { _text->setColor(*accessory.color); }
  _accessory = accessory;
}

ListAccessoryWidget::ListAccessoryWidget(QWidget *parent)
    : QWidget(parent), _layout(new QHBoxLayout), _icon(new ImageWidget), _text(new TypographyWidget()),
      _tooltip(new TooltipWidget) {
  _layout->setContentsMargins(6, 3, 6, 3);
  _layout->setAlignment(Qt::AlignVCenter);
  _layout->addWidget(_icon);
  _layout->addWidget(_text);
  _icon->setFixedSize(16, 16);
  _text->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setLayout(_layout);
}

ListAccessoryWidget::~ListAccessoryWidget() { _tooltip->deleteLater(); }
