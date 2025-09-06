#pragma once
#include "../image/url.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qgraphicseffect.h>
#include <qwidget.h>

class OmniButtonWidget : public QWidget {
public:
  enum ButtonColor { Primary, Secondary, Destructive, Transparent };

  Q_OBJECT

  QWidget *leftAccessory = new QWidget;
  QWidget *rightAccessory = new QWidget;
  QHBoxLayout *_layout = new QHBoxLayout(this);
  TypographyWidget *label = new TypographyWidget;
  bool _focused = false;
  bool m_disabled = false;
  ColorLike m_color = Qt::transparent;
  ColorLike m_hoverColor = Qt::transparent;
  QGraphicsOpacityEffect *m_opacityEffect = new QGraphicsOpacityEffect(this);

protected:
  void paintEvent(QPaintEvent *event) override;
  void focusInEvent(QFocusEvent *event) override { setFocused(true); }
  void focusOutEvent(QFocusEvent *event) override { setFocused(false); }
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  bool event(QEvent *event) override { return QWidget::event(event); }
  void keyPressEvent(QKeyEvent *event) override;

public:
  void setBackgroundColor(const ColorLike &color);
  void setDisabled(bool disabled);
  void setHoverBackgroundColor(const ColorLike &color);
  void setFocused(bool value);
  void setLeftAccessory(QWidget *w);
  void setRightAccessory(QWidget *w);
  void setLeftIcon(const ImageURL &url, QSize size = {25, 25});
  void setRightAccessory(const ImageURL &url, QSize size = {25, 25});
  void setColor(const ColorLike &color);
  void setText(const QString &text);
  void setColor(ButtonColor color);

  OmniButtonWidget(QWidget *parent = nullptr);

signals:
  void clicked() const;
  void doubleClicked() const;
  void activated() const;
};
