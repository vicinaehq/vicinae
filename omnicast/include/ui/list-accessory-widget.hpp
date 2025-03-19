#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/ellided-label.hpp"
#include "ui/tooltip.hpp"
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
  OmniIcon *_icon = nullptr;
  EllidedLabel *_label = nullptr;
  Tooltip *_tooltip;
  ListAccessory _accessory;

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  void setAccessory(const ListAccessory &accessory);

  ListAccessoryWidget(QWidget *parent = nullptr);
  ~ListAccessoryWidget();
};
