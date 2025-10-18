#pragma once
#include <qapplication.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <LayerShellQt/Window>
#include <qtimer.h>
#include <qwidget.h>

class Popover : public QWidget {
public:
  Popover(QWidget *parent = nullptr);

  void paintEvent(QPaintEvent *event) override;
};
