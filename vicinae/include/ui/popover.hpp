#pragma once
#include <qapplication.h>
#include <qdatetime.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <LayerShellQt/Window>
#include <qtimer.h>
#include <qwidget.h>

class Popover : public QWidget {
  QPixmap _wallpaper;

public:
  Popover(QWidget *parent = nullptr);

  void recomputeWallpaper();
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
};
