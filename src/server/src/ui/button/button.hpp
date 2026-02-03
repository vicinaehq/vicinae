#pragma once
#include <qboxlayout.h>
#include <qgraphicseffect.h>
#include <qwidget.h>
#include "../image/url.hpp"
#include "theme.hpp"
#include "ui/button-base/button-base.hpp"
#include "ui/typography/typography.hpp"

class ButtonWidget : public ButtonBase {
  Q_OBJECT

public:
  enum ButtonColor { Primary, Secondary, Transparent };

  ButtonWidget(QWidget *parent = nullptr);

  void setLeftAccessory(QWidget *w);
  void setRightAccessory(QWidget *w);
  void setLeftIcon(const ImageURL &url, QSize size = {25, 25});
  void setRightAccessory(const ImageURL &url, QSize size = {25, 25});
  void setText(const QString &text);
  void setTextColor(const ColorLike &color);

private:
  QWidget *leftAccessory = new QWidget;
  QWidget *rightAccessory = new QWidget;
  TypographyWidget *label = new TypographyWidget;
};
