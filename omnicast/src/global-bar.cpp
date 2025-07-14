#include "global-bar.hpp"
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qwidget.h>

NavigationStatusWidget::NavigationStatusWidget() { setupUI(); }

void NavigationStatusWidget::setTitle(const QString &title) { m_navigationTitle->setText(title); }
void NavigationStatusWidget::setIcon(const OmniIconUrl &icon) { m_navigationIcon->setUrl(icon); }

void NavigationStatusWidget::setupUI() {
  auto layout = new QVBoxLayout;

  m_navigationIcon->setFixedSize(25, 25);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_navigationIcon);
  layout->addWidget(m_navigationTitle);
  setLayout(layout);
}

GlobalBar::GlobalBar(NavigationController &nav) : m_navigation(nav) { setupUI(); }

void GlobalBar::setupUI() {
  setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(10, 0, 10, 0);
  layout->setSpacing(0);
  m_leftWidget->addWidget(m_status);
  layout->addWidget(m_leftWidget);
  m_status->setIcon(BuiltinOmniIconUrl("omnicast"));

  setLayout(layout);
}
