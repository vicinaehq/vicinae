#include "ui/icon-button/icon-button.hpp"
#include "ui/image/image.hpp"

void IconButton::resizeEvent(QResizeEvent *event) {
  auto margins = contentsMargins();
  auto iconRect = rect().marginsRemoved(contentsMargins());

  _icon->setFixedSize(iconRect.size());
  _icon->move(margins.left(), margins.top());
  OmniButtonWidget::resizeEvent(event);
}

IconButton::IconButton() : _icon(new ImageWidget(this)) {
  _icon->setContentsMargins(3, 3, 3, 3);
  _icon->show();
}

void IconButton::setUrl(const ImageURL &url) { _icon->setUrl(url); }
