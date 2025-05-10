#include "ui/icon-button.hpp"
#include "ui/image/omnimg.hpp"

void IconButton::resizeEvent(QResizeEvent *event) {
  auto margins = contentsMargins();
  auto iconRect = rect().marginsRemoved(contentsMargins());

  _icon->setFixedSize(iconRect.size());
  _icon->move(margins.left(), margins.top());
  Button::resizeEvent(event);
}

IconButton::IconButton() : _icon(new Omnimg::ImageWidget(this)) {
  setContentsMargins(3, 3, 3, 3);
  _icon->show();
}

void IconButton::setUrl(const OmniIconUrl &url) { _icon->setUrl(url); }
