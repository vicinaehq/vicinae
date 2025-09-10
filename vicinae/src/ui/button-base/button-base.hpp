#pragma once
#include "theme.hpp"
#include <qboxlayout.h>
#include <qgraphicseffect.h>
#include <qwidget.h>

/**
 * Base class most button-like things inherit from.
 * This provides proper hover, click, and keyboard handling.
 * This widget does not set any layout so that the inheriting class
 * can set its own.
 */
class ButtonBase : public QWidget {
public:
  enum ButtonColor { Primary, Secondary, Transparent };

  ButtonBase(QWidget *parent = nullptr);

  void setColor(ButtonColor color);

  void setDisabled(bool disabled);
  void setBackgroundColor(const ColorLike &color);
  void setHoverBackgroundColor(const ColorLike &color);
  void setFocused(bool value);

signals:
  void clicked() const;
  void doubleClicked() const;
  void activated() const;
  void hoverChanged(bool value) const;

protected:
  void paintEvent(QPaintEvent *event) override;
  void focusInEvent(QFocusEvent *event) override { setFocused(true); }
  void focusOutEvent(QFocusEvent *event) override { setFocused(false); }
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  bool event(QEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private:
  Q_OBJECT

  bool m_focused = false;
  bool m_disabled = false;
  ColorLike m_color = Qt::transparent;
  ColorLike m_hoverColor = Qt::transparent;
  QGraphicsOpacityEffect *m_opacityEffect = new QGraphicsOpacityEffect(this);
};
