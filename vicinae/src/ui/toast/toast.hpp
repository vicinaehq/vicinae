#pragma once
#include "ui/color-circle/color_circle.hpp"
#include "ui/spinner/spinner.hpp"
#include "ui/typography/typography.hpp"
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <qstackedwidget.h>

class Toast;
enum ToastStyle { Success, Info, Warning, Danger, Dynamic };

class ToastWidget : public QWidget {

public:
  ToastWidget();

  void setToast(const Toast *toast);

private:
  TypographyWidget *m_text = new TypographyWidget;
  ColorCircle *m_circle = new ColorCircle({10, 10}, this);
  LoadingSpinner *m_spinner = new LoadingSpinner(this);
};
