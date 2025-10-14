#pragma once
#include "theme.hpp"
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <QPen>

class LoadingSpinner : public QWidget {
public:
  explicit LoadingSpinner(QWidget *parent = nullptr);

  void start();
  void stop();
  void setColor(const ColorLike &color);
  void setSize(int size);
  void setThickness(int thickness);

protected:
  void paintEvent(QPaintEvent *event) override;
  void rotate();

private:
  QTimer *m_timer;
  int m_size;
  int m_thickness;
  int m_startAngle;
  ColorLike m_color;
};
