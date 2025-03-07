#include "ui/toast.hpp"
#include "ui/color_circle.hpp"

static QHash<ToastPriority, QColor> toastPriorityToColor{
    {ToastPriority::Success, QColor("green")},
    {ToastPriority::Info, QColor("cyan")},
    {ToastPriority::Warning, QColor("orange")},
    {ToastPriority::Danger, QColor("red")},
};

ToastWidget::ToastWidget(const QString &text, ToastPriority priority) {
  auto layout = new QHBoxLayout();
  auto circle = new ColorCircle(toastPriorityToColor[priority], {10, 10}, this);

  layout->addWidget(circle);
  layout->addWidget(new QLabel(text));
  layout->setContentsMargins(0, 0, 0, 0);

  fadeOutTimer.setSingleShot(true);
  connect(&fadeOutTimer, &QTimer::timeout, [this]() { emit fadeOut(); });

  setLayout(layout);
}

void ToastWidget::start(int duration) { fadeOutTimer.start(duration); }
