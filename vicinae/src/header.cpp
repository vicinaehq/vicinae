#include "header.hpp"
#include "navigation-controller.hpp"
#include "vicinae.hpp"
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <qwidget.h>
#include "base-view.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "utils/layout.hpp"

void GlobalHeader::setupUI() {

  m_input = new SearchBar(this);
  m_backButton = new IconButton;
  m_accessoryContainer = new QStackedWidget(this);
  m_loadingBar = new HorizontalLoadingBar(this);

  m_backButton->setFixedSize(25, 25);
  m_backButton->setFocusPolicy(Qt::NoFocus);
  m_backButton->setBackgroundColor(SemanticColor::MainSelectedBackground);
  m_backButton->setUrl(BuiltinOmniIconUrl("arrow-left"));
  m_backButtonSpacer->setFixedWidth(10);
  m_backButtonSpacer->hide();
  m_backButton->hide();

  m_loadingBar->setFixedHeight(1);
  m_loadingBar->setBarWidth(100);

  setFixedHeight(Omnicast::TOP_BAR_HEIGHT);

  auto left = HStack()
                  .add(m_backButton)
                  .add(m_backButtonSpacer)
                  .add(m_input, 1)
                  .addStretch()
                  .add(m_accessoryContainer, 0, Qt::AlignVCenter);

  VStack().add(left.margins(15, 5, 15, 5)).add(m_loadingBar).imbue(this);

  m_input->installEventFilter(this);
  m_input->setFocus();
}

void GlobalHeader::setAccessory(QWidget *accessory) {
  clearAccessory();
  m_accessoryContainer->addWidget(accessory);
}

void GlobalHeader::clearAccessory() {
  if (auto widget = m_accessoryContainer->widget(0)) { m_accessoryContainer->removeWidget(widget); }
}

SearchBar *GlobalHeader::input() const { return m_input; }

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
  if (!m_input->isVisible()) return;

  if (m_navigation.viewStackSize() > 1) { m_navigation.popCurrentView(); }
}

GlobalHeader::GlobalHeader(NavigationController &controller) : m_navigation(controller) {
  setupUI();
  connect(&m_navigation, &NavigationController::searchTextSelected, m_input, &SearchBar::selectAll);
  connect(&m_navigation, &NavigationController::currentViewChanged, this, [this]() {
    if (auto state = m_navigation.topState()) {
      bool needsBackButton = m_navigation.viewStackSize() > 1;

      m_backButton->setVisible(needsBackButton);
      m_backButtonSpacer->setVisible(needsBackButton);
    }
  });
  connect(&m_navigation, &NavigationController::searchAccessoryChanged, this, &GlobalHeader::setAccessory);
  connect(&m_navigation, &NavigationController::searchAccessoryCleared, this, &GlobalHeader::clearAccessory);
  connect(&m_navigation, &NavigationController::searchTextChanged, m_input, &SearchBar::setText);
  connect(&m_navigation, &NavigationController::searchPlaceholderTextChanged, m_input,
          &SearchBar::setPlaceholderText);
  connect(m_input, &SearchBar::pop, this, &GlobalHeader::handleSearchPop);
  connect(m_backButton, &IconButton::clicked, &m_navigation, &NavigationController::popCurrentView);
  connect(m_input, &SearchBar::textEdited, this, &GlobalHeader::handleTextEdited);
  connect(&m_navigation, &NavigationController::loadingChanged, m_loadingBar,
          &HorizontalLoadingBar::setStarted);
}
