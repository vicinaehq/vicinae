#include "header.hpp"
#include "navigation-controller.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qwidget.h>

void GlobalHeader::handleViewStateChange(const NavigationController::ViewState &state) {
  m_input->setText(state.text);
  m_input->setPlaceholderText(state.placeholderText);
}

void GlobalHeader::setupUI() {
  auto vlayout = new QVBoxLayout;
  auto hlayout = new QHBoxLayout;
  auto horizontalWidget = new QWidget;

  hlayout->setContentsMargins(10, 0, 10, 0);
  hlayout->addWidget(m_input);
  horizontalWidget->setLayout(hlayout);

  vlayout->setContentsMargins(0, 0, 0, 0);
  vlayout->setSpacing(0);
  vlayout->addWidget(horizontalWidget);
  vlayout->addWidget(m_loadingBar);

  setFixedHeight(Omnicast::TOP_BAR_HEIGHT);
  setLayout(vlayout);
  m_input->installEventFilter(this);
  m_input->setFocus();
}

bool GlobalHeader::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_input) {
    if (auto state = m_navigation.topState()) {
      if (auto filtered = state->sender->eventFilter(watched, event)) { return filtered; }
    }
  }

  return QWidget::eventFilter(watched, event);
}

void GlobalHeader::handleSearchPop() {
  if (m_navigation.viewStackSize() > 1) { m_navigation.popCurrentView(); }
}

GlobalHeader::GlobalHeader(NavigationController &controller) : m_navigation(controller) {
  setupUI();
  connect(&m_navigation, &NavigationController::currentViewStateChanged, this,
          &GlobalHeader::handleViewStateChange);
  connect(m_input, &SearchBar::pop, this, &GlobalHeader::handleSearchPop);
}
