#pragma once
#include <qobject.h>
#include <qwindow.h>

class AbstractBackgroundEffectManager : public QObject {
public:
  struct BlurConfig {
    int radius = 0;
    QRect region;

    bool operator==(const BlurConfig &rhs) const { return radius == rhs.radius && region == rhs.region; }
  };

  virtual ~AbstractBackgroundEffectManager() = default;

  virtual bool supportsBlur() const = 0;
  virtual bool setBlur(QWindow *win, const BlurConfig &cfg) = 0;
  virtual bool removeBlur(QWindow *win) = 0;
};
