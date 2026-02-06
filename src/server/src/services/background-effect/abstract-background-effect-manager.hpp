#pragma once
#include <qwindow.h>

class AbstractBackgroundEffectManager {

public:
  struct BlurConfig {
    int radius = 0;
    QRect region;
  };

  virtual ~AbstractBackgroundEffectManager() = default;

  virtual bool supportsBlur() const = 0;
  virtual bool setBlur(QWindow *win, const BlurConfig &cfg) = 0;
  virtual bool removeBlur(QWindow *win) = 0;
};
