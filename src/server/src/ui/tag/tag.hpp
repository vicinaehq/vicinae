#pragma once
#include "../image/url.hpp"
#include "theme.hpp"
#include "ui/image/image.hpp"
#include <qevent.h>
#include <qwidget.h>

class TypographyWidget;

class TagWidget : public QWidget {
  std::optional<ColorLike> m_color;
  TypographyWidget *m_text;
  ImageWidget *m_image;

  void setupUI();

  void paintEvent(QPaintEvent *event) override;

public:
  TagWidget();

  void setColor(const std::optional<ColorLike> &color);
  void setText(const QString &text);
  void setIcon(const ImageURL &icon);
};

using ChipWidget = TagWidget;
