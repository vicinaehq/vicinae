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
  void closeEvent(QCloseEvent *event) override {
    QWidget::closeEvent(event);

    qDebug() << "close popover";

#ifdef WAYLAND_LAYER_SHELL
    QTimer::singleShot(100, [&]() {
      for (const auto &window : QApplication::allWindows()) {
        if (window->objectName() == "AppWindowClassWindow") {
          if (auto lshell = LayerShellQt::Window::get(window)) {
            qDebug() << "Reapplying layer shell surface";
            // lshell->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
            // lshell->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityExclusive);
          } else {
            qCritical()
                << "Unable apply layer shell rules to main window: LayerShellQt::Window::get() returned null";
          }

          window->requestActivate();
          window->raise();
        }
      }
    });

#endif
  }
};
