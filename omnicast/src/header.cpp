#include "header.hpp"
#include "navigation-controller.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qwidget.h>
#include "base-view.hpp"

void GlobalHeader::handleViewStateChange(const NavigationController::ViewState &state) {
  qDebug() << "view state change" << state.searchText << state.placeholderText;
  m_input->setText(state.searchText);
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

void GlobalHeader::handleTextEdited(const QString &text) { m_navigation.setSearchText(text); }

bool GlobalHeader::filterInputEvents(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (auto state = m_navigation.topState()) {
      if (state->sender->inputFilter(keyEvent)) { return true; }
    }
  }

  return false;
}

bool GlobalHeader::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_input) {
    if (filterInputEvents(event)) { return true; }
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
  connect(m_input, &SearchBar::textEdited, this, &GlobalHeader::handleTextEdited);
}
