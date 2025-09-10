#include "button.hpp"
#include "ui/button-base/button-base.hpp"
#include "ui/image/image.hpp"
#include "utils/layout.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qpainterpath.h>

void OmniButtonWidget::setLeftAccessory(QWidget *w) {
  if (auto leftItem = layout()->itemAt(0)) {
    layout()->replaceWidget(leftItem->widget(), w);
    leftItem->widget()->deleteLater();
  }
}

void OmniButtonWidget::setRightAccessory(QWidget *w) {
  if (auto rightItem = layout()->itemAt(2)) {
    layout()->replaceWidget(rightItem->widget(), w);
    rightItem->widget()->deleteLater();
  }
}

void OmniButtonWidget::setTextColor(const ColorLike &color) { label->setColor(color); }

void OmniButtonWidget::setLeftIcon(const ImageURL &url, QSize size) {
  auto icon = new ImageWidget();

  icon->setFixedSize(size);
  icon->setUrl(url);
  setLeftAccessory(icon);
}

void OmniButtonWidget::setText(const QString &text) { label->setText(text); }

void OmniButtonWidget::setRightAccessory(const ImageURL &url, QSize size) {
  auto icon = new ImageWidget;

  icon->setFixedSize(size);
  icon->setUrl(url);
  setRightAccessory(icon);
}

OmniButtonWidget::OmniButtonWidget(QWidget *parent) : ButtonBase(parent) {
  auto layout = HStack()
                    .margins(5)
                    .spacing(5)
                    .add(leftAccessory)
                    .add(label)
                    .add(rightAccessory, 0, Qt::AlignRight)
                    .buildLayout();

  layout->setAlignment(Qt::AlignHCenter);
  setLayout(layout);
}
