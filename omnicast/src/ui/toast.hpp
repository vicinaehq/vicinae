#pragma once
#include <QColor>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

enum ToastPriority { Success, Info, Warning, Danger };

class ToastWidget : public QWidget {
  Q_OBJECT
  QTimer fadeOutTimer;

public:
  ToastWidget(const QString &text, ToastPriority priority = ToastPriority::Success);

  void start(int duration = 1000);

signals:
  void fadeOut();
};
