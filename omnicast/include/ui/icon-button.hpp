#pragma once
#include "omni-icon.hpp"
#include "ui/button.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>

class IconButton : public Button {
  OmniIcon *_icon;

  void resizeEvent(QResizeEvent *event) {
    auto margins = contentsMargins();
    auto iconRect = rect().marginsRemoved(contentsMargins());

    _icon->setFixedSize(iconRect.size());
    _icon->move(margins.left(), margins.top());
    Button::resizeEvent(event);
  }

public:
  IconButton() : _icon(new OmniIcon(this)) {
    setContentsMargins(3, 3, 3, 3);
    _icon->show();
  }

  void setUrl(const OmniIconUrl &url) { _icon->setUrl(url); }
};
