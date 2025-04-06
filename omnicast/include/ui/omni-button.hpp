#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qwidget.h>

class OmniButtonWidget : public QWidget {
  QWidget *leftAccessory = new QWidget;
  QWidget *rightAccessory = new QWidget;
  TypographyWidget *label = new TypographyWidget;
  QHBoxLayout *_layout = new QHBoxLayout;

protected:
  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 4;
    QPainter painter(this);
    QPainterPath path;
    QPen pen(theme.colors.statusBackgroundBorder, 1);

    painter.setRenderHint(QPainter::Antialiasing, true);
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor finalColor(theme.colors.mainBackground);

    painter.setPen(Qt::NoPen);
    painter.fillPath(path, finalColor);
    painter.drawPath(path);
  }

public:
  void setContentsMargins(int left, int top, int right, int bottom) {
    _layout->setContentsMargins(left, top, right, bottom);
  }

  void setContentsMargins(QMargins margins) { _layout->setContentsMargins(margins); }

  void setLeftAccessory(QWidget *w) {
    if (auto leftItem = _layout->itemAt(0)) {
      _layout->replaceWidget(leftItem->widget(), w);
      leftItem->widget()->deleteLater();
    }
  }

  void setRightAccessory(QWidget *w) {
    if (auto rightItem = _layout->itemAt(2)) {
      _layout->replaceWidget(rightItem->widget(), w);
      rightItem->widget()->deleteLater();
    }
  }

  void setLeftIcon(const OmniIconUrl &url, QSize size = {25, 25}) {
    auto icon = new OmniIcon(url);

    icon->setFixedSize(size);
    setLeftAccessory(icon);
  }

  void setRightAccessory(const OmniIconUrl &url, QSize size = {25, 25}) {
    auto icon = new OmniIcon(url);

    icon->setFixedSize(size);
    setRightAccessory(icon);
  }

  void setColor(const ColorLike &color) { label->setColor(color); }

  void setText(const QString &text) { label->setText(text); }

  OmniButtonWidget(QWidget *parent = nullptr) : QWidget(parent) {
    setAttribute(Qt::WA_Hover, true);

    _layout->setContentsMargins(5, 5, 5, 5);
    _layout->addWidget(leftAccessory);
    _layout->addWidget(label);
    _layout->addWidget(rightAccessory, 0, Qt::AlignRight);

    setLayout(_layout);
  }
};
