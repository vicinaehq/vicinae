#pragma once
#include <chrono>
#include <qtimer.h>
#include <qwidget.h>

class HorizontalLoadingBar : public QWidget {
  bool _isAnimationStarted;
  QTimer *_timer;
  bool m_started = false;
  int _position;
  int _positionStep;
  int _barWidth;
  std::chrono::milliseconds _tick;
  QTimer m_loadingDebounce;
  void paintEvent(QPaintEvent *event) override;
  void updateAnimation();

public:
  void setBarWidth(int width);
  void setPositionStep(int step);
  void setTick(std::chrono::milliseconds ms);
  void start();
  void stop();
  void setStarted(bool started);

  HorizontalLoadingBar(QWidget *parent = nullptr);
};
