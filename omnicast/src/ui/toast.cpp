#include "ui/toast.hpp"
#include "services/toast/toast-service.hpp"
#include "theme.hpp"
#include "ui/color_circle.hpp"
#include "ui/typography/typography.hpp"
#include <qnamespace.h>

static QHash<ToastPriority, ColorLike> toastPriorityToColor{
    {ToastPriority::Success, SemanticColor::Green},
    {ToastPriority::Info, SemanticColor::Blue},
    {ToastPriority::Warning, SemanticColor::Orange},
    {ToastPriority::Danger, SemanticColor::Red},
};

ToastWidget::ToastWidget() {
  auto layout = new QHBoxLayout();

  layout->addWidget(m_circle, 0, Qt::AlignVCenter);
  layout->addWidget(m_text, 0, Qt::AlignVCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
}

void ToastWidget::setToast(const Toast *toast) {
  m_text->setText(toast->title());
  m_circle->setColor(toastPriorityToColor[toast->priority()]);
}
