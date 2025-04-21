#include "ui/toast.hpp"
#include "theme.hpp"
#include "ui/color_circle.hpp"
#include "ui/typography.hpp"
#include <qnamespace.h>

static QHash<ToastPriority, ColorLike> toastPriorityToColor{
    {ToastPriority::Success, ColorTint::Green},
    {ToastPriority::Info, ColorTint::Blue},
    {ToastPriority::Warning, ColorTint::Orange},
    {ToastPriority::Danger, ColorTint::Red},
};

ToastWidget::ToastWidget(const QString &text, ToastPriority priority) {
  auto layout = new QHBoxLayout();
  auto circle = new ColorCircle(toastPriorityToColor[priority], {10, 10}, this);
  auto typo = new TypographyWidget;

  typo->setText(text);

  layout->addWidget(circle, 0, Qt::AlignVCenter);
  layout->addWidget(typo, 0, Qt::AlignVCenter);
  layout->setContentsMargins(0, 0, 0, 0);

  fadeOutTimer.setSingleShot(true);
  connect(&fadeOutTimer, &QTimer::timeout, [this]() { emit fadeOut(); });

  setLayout(layout);
}

void ToastWidget::start(int duration) { fadeOutTimer.start(duration); }
