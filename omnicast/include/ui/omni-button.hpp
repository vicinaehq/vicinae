#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class OmniButtonWidget : public QWidget {
  Q_OBJECT

  QWidget *leftAccessory = new QWidget;
  QWidget *rightAccessory = new QWidget;
  TypographyWidget *label = new TypographyWidget;
  QHBoxLayout *_layout = new QHBoxLayout;
  bool _focused = false;
  bool m_hovered = false;
  ColorLike m_color = Qt::transparent;
  ColorLike m_hoverColor = Qt::transparent;

protected:
  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 4;
    OmniPainter painter(this);
    QPainterPath path;
    QPen pen(theme.colors.text, 1);
    QBrush brush;

    if (underMouse()) {
      qDebug() << "under mouse";
      brush = painter.colorBrush(m_hoverColor);
    } else {
      brush = painter.colorBrush(m_color);
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    path.addRoundedRect(rect(), borderRadius, borderRadius);
    painter.setClipPath(path);
    painter.setPen(_focused ? pen : Qt::NoPen);
    painter.fillPath(path, brush);
    painter.drawPath(path);
  }

  void focusInEvent(QFocusEvent *event) override { setFocused(true); }

  void focusOutEvent(QFocusEvent *event) override { setFocused(false); }

  void mouseDoubleClickEvent(QMouseEvent *event) override {
    emit doubleClicked();
    emit activated();
  }

  void mousePressEvent(QMouseEvent *event) override {
    emit clicked();
    emit activated();
  }

  bool event(QEvent *event) override { return QWidget::event(event); }

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      emit activated();
      return;
    }

    QWidget::keyPressEvent(event);
  }

public:
  void setBackgroundColor(const ColorLike &color) {
    m_color = color;
    update();
  }

  void setHoverBackgroundColor(const ColorLike &color) {
    m_hoverColor = color;
    update();
  }

  void setFocused(bool value) {
    if (_focused == value) return;

    _focused = value;
    update();
  }

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
    setAttribute(Qt::WA_Hover);
    setFocusPolicy(Qt::StrongFocus);

    _layout->setContentsMargins(5, 5, 5, 5);
    _layout->addWidget(leftAccessory);
    _layout->addWidget(label);
    _layout->addWidget(rightAccessory, 0, Qt::AlignRight);

    setLayout(_layout);
  }

signals:
  void clicked() const;
  void doubleClicked() const;
  void activated() const;
};
