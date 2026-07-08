#pragma once
#include <qobject.h>
#include <qwindow.h>

/**
 * Platform backend that applies a "material" to a window background: a frosted
 * blur on Wayland (via ext-background-effect / kde-blur) or vibrancy on macOS
 * (via NSVisualEffectView). Implementations are selected by WindowMaterialManager.
 */
class WindowMaterialBackend : public QObject {
public:
  struct Params {
    int radius = 0;
    QRect region;

    bool operator==(const Params &rhs) const { return radius == rhs.radius && region == rhs.region; }
  };

  ~WindowMaterialBackend() override = default;

  virtual bool isSupported() const = 0;
  virtual bool apply(QWindow *win, const Params &params) = 0;
  virtual bool clear(QWindow *win) = 0;
};
