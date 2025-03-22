#pragma once
#include <qpixmap.h>
#include <qwidget.h>

class Popover : public QWidget {
  QPixmap _wallpaper;

public:
  Popover(QWidget *parent = nullptr);

  void recomputeWallpaper();
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};
