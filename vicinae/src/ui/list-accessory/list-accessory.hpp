#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/typography/typography.hpp"
#include "ui/tooltip/tooltip.hpp"
#include <optional>
#include <qcontainerfwd.h>

struct ListAccessory {
  QString text;
  std::optional<ColorLike> color;
  QString tooltip;
  bool fillBackground;
  std::optional<OmniIconUrl> icon;
};

class ListAccessoryWidget : public QWidget {
  QHBoxLayout *_layout;
  Omnimg::ImageWidget *_icon = nullptr;
  TooltipWidget *_tooltip;
  ListAccessory _accessory;
  TypographyWidget *_text;

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  void setAccessory(const ListAccessory &accessory);

  ListAccessoryWidget(QWidget *parent = nullptr);
  ~ListAccessoryWidget();
};
