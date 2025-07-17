#include "header.hpp"
#include "navigation-controller.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "base-view.hpp"

void GlobalHeader::setupUI() {
  auto vlayout = new QVBoxLayout;
  auto hlayout = new QHBoxLayout;
  auto horizontalWidget = new QWidget;

  hlayout->setContentsMargins(15, 5, 15, 5);
  hlayout->addWidget(m_backButton);
  hlayout->addWidget(m_backButtonSpacer);
  hlayout->addWidget(m_input, 1);
  hlayout->addWidget(m_accessoryContainer);
  hlayout->setSpacing(0);
  hlayout->setAlignment(Qt::AlignVCenter);

  m_backButton->setFixedSize(25, 25);
  m_backButton->setFocusPolicy(Qt::NoFocus);
  m_backButton->setBackgroundColor(ColorTint::MainSelectedBackground);
  m_backButton->setUrl(BuiltinOmniIconUrl("arrow-left"));
  m_backButtonSpacer->setFixedWidth(10);
  m_backButtonSpacer->hide();
  m_backButton->hide();

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
  connect(m_backButton, &IconButton::clicked, this, &GlobalHeader::handleSearchPop);
  connect(m_input, &SearchBar::textEdited, this, &GlobalHeader::handleTextEdited);
}
