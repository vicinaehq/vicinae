#include "ui/toast/toast.hpp"
#include "layout.hpp"
#include "services/toast/toast-service.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/color-circle/color_circle.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "ui/typography/typography.hpp"
#include "utils/utils.hpp"
#include <qlogging.h>
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
  m_spinner->setColor(OmniPainter::resolveColor(SemanticColor::Foreground));
  m_spinner->hide();
  m_circle->hide();
  m_spinner->setThickness(2);
  m_title->setAutoEllide(false);

  auto layout = HStack().spacing(5).add(m_spinner).add(m_circle).add(m_title, 1).buildLayout();
  layout->setAlignment(Qt::AlignVCenter);
  setLayout(layout);
}

void ToastWidget::setToast(const Toast *toast) {
  const auto secondary = OmniPainter::resolveColor(SemanticColor::TextMuted);
  QString text;

  if (!toast->title().isEmpty()) { text.append(toast->title()); }
  if (!toast->message().isEmpty()) {
    QString fmt = QString(R"(<span style="color: %1;">%2</span>)")
                      .arg(Utils::rgbaFromColor(secondary))
                      .arg(toast->message());
    if (!text.isEmpty()) text.append(' ');
    text.append(fmt);
  }

  m_title->setText(text);
  m_title->setVisible(!text.isEmpty());
  m_circle->setVisible(toast->priority() != ToastStyle::Dynamic);
  m_spinner->setVisible(toast->priority() == ToastStyle::Dynamic);

  if (toast->priority() == ToastStyle::Dynamic) {
    m_spinner->start();
    return;
  }

  m_circle->setColor(toastPriorityToColor[toast->priority()]);
}
