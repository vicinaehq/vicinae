#pragma once
#include <qapplication.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#ifdef WAYLAND_LAYER_SHELL
#include <LayerShellQt/Window>
#endif
#include <qtimer.h>
#include <qwidget.h>

class Popover : public QWidget {
public:
  Popover(QWidget *parent = nullptr);

  void paintEvent(QPaintEvent *event) override;
};
