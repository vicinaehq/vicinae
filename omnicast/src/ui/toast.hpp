#pragma once
#include "ui/color_circle.hpp"
#include "ui/typography.hpp"
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

class Toast;
enum ToastPriority { Success, Info, Warning, Danger };

class ToastWidget : public QWidget {
  TypographyWidget *m_text = new TypographyWidget;
  ColorCircle *m_circle = new ColorCircle({10, 10}, this);

public:
  ToastWidget();

  void setToast(const Toast *toast);
};
