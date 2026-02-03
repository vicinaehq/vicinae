#include "ui/icon-button/icon-button.hpp"
#include "ui/image/image.hpp"
#include "utils/layout.hpp"
#include <qnamespace.h>

IconButton::IconButton() : m_icon(new ImageWidget(this)) {
  setFocusPolicy(Qt::NoFocus);
  HStack().add(m_icon).margins(3).imbue(this);
}

void IconButton::resizeEvent(QResizeEvent *event) {
  m_icon->setFixedSize(rect().marginsRemoved(layout()->contentsMargins()).size());
  ButtonBase::resizeEvent(event);
};

void IconButton::setUrl(const ImageURL &url) { m_icon->setUrl(url); }
