#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include <qevent.h>
#include <qwidget.h>

class TypographyWidget;

class TagWidget : public QWidget {
  std::optional<ColorLike> m_color;
  TypographyWidget *m_text;
  Omnimg::ImageWidget *m_image;

  void setupUI();

  void paintEvent(QPaintEvent *event) override;

public:
  TagWidget();

  void setColor(const std::optional<ColorLike> &color);
  void setText(const QString &text);
  void setIcon(const OmniIconUrl &icon);
};

using ChipWidget = TagWidget;
