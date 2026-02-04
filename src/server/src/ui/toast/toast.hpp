#pragma once
#include "ui/color-circle/color_circle.hpp"
#include "ui/spinner/spinner.hpp"
#include "ui/typography/typography.hpp"
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <cstdint>
#include <qstackedwidget.h>

class Toast;
enum ToastStyle : std::uint8_t { Success, Info, Warning, Danger, Dynamic };

class ToastWidget : public QWidget {

public:
  ToastWidget();

  void setToast(const Toast *toast);

private:
  TypographyWidget *m_title = new TypographyWidget;
  ColorCircle *m_circle = new ColorCircle({10, 10}, this);
  LoadingSpinner *m_spinner = new LoadingSpinner(this);
};
