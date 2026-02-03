#include "ui/toast/toast.hpp"
#include "layout.hpp"
#include "services/toast/toast-service.hpp"
#include "theme.hpp"
#include "ui/color-circle/color_circle.hpp"
#include "ui/typography/typography.hpp"
#include <qnamespace.h>

static QHash<ToastStyle, ColorLike> toastPriorityToColor{
    {ToastStyle::Success, SemanticColor::Green},
    {ToastStyle::Info, SemanticColor::Blue},
    {ToastStyle::Warning, SemanticColor::Orange},
    {ToastStyle::Danger, SemanticColor::Red},
};

ToastWidget::ToastWidget() {
  OmniPainter painter;

  m_spinner->setSize(12);
  m_spinner->setColor(painter.resolveColor(SemanticColor::Foreground));
  m_spinner->hide();
  m_circle->hide();
  m_spinner->setThickness(2);

  auto layout = HStack().spacing(5).add(m_spinner).add(m_circle).add(m_text).buildLayout();
  layout->setAlignment(Qt::AlignVCenter);
  setLayout(layout);
}

void ToastWidget::setToast(const Toast *toast) {
  m_text->setText(toast->title());
  m_circle->setVisible(toast->priority() != ToastStyle::Dynamic);
  m_spinner->setVisible(toast->priority() == ToastStyle::Dynamic);

  if (toast->priority() == ToastStyle::Dynamic) {
    m_spinner->start();
    return;
  }

  m_circle->setColor(toastPriorityToColor[toast->priority()]);
}
