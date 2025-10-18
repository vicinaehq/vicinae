#include "top-bar.hpp"
#include "keyboard/keybind-manager.hpp"
#include "navigation-controller.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/arg-completer/arg-completer.hpp"
#include "vicinae.hpp"
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <qwidget.h>
#include "ui/views/base-view.hpp"
#include "ui/loading-bar/horizontal-loading-bar.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "utils/layout.hpp"
#include "ui/search-bar/search-bar.hpp"

void GlobalHeader::setupUI() {
  setAttribute(Qt::WA_TranslucentBackground);
  m_input = new SearchBar(this);
  m_backButton = new IconButton;
  m_accessoryContainer = new QStackedWidget(this);
  m_loadingBar = new HorizontalLoadingBar(this);
  m_completer = new ArgCompleter(this);

  m_backButton->setFixedSize(25, 25);
  m_backButton->setFocusPolicy(Qt::NoFocus);
  m_backButton->setUrl(ImageURL::builtin("arrow-left"));
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
                  .add(m_completer)
                  .addStretch()
                  .add(m_accessoryContainer, 0, Qt::AlignVCenter);

  m_completer->hide();

  VStack().add(left.margins(15, 5, 15, 5)).add(m_loadingBar).imbue(this);

  m_input->installEventFilter(this);
  m_input->setFocus();

  connect(&m_navigation, &NavigationController::completionCreated, this, [this](const CompleterState &state) {
    m_input->setInline(true);
    m_completer->setIconUrl(state.icon);
    m_completer->setArguments(state.args);
  });

  connect(&m_navigation, &NavigationController::completionDestroyed, this, [this]() {
    m_input->setInline(false);
    m_completer->clear();
  });

  connect(&m_navigation, &NavigationController::invalidCompletionFired, m_completer, &ArgCompleter::validate);
  connect(&m_navigation, &NavigationController::completionValuesChanged, m_completer,
          &ArgCompleter::setValues);

  connect(m_completer, &ArgCompleter::valueChanged, this,
          [this](auto &&values) { m_navigation.setCompletionValues(values); });
}

void GlobalHeader::setAccessory(QWidget *accessory) {
  clearAccessory();
  m_accessoryContainer->addWidget(accessory);
  m_accessoryContainer->setCurrentWidget(accessory);
}

void GlobalHeader::clearAccessory() {
  if (auto widget = m_accessoryContainer->widget(0)) { m_accessoryContainer->removeWidget(widget); }
}

SearchBar *GlobalHeader::input() const { return m_input; }

void GlobalHeader::handleTextEdited(const QString &text) { m_navigation.setSearchText(text); }

bool GlobalHeader::filterInputEvents(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (KeybindManager::instance()->resolve(Keybind::ToggleActionPanel) == keyEvent) {
      m_navigation.toggleActionPanel();
      return true;
    }

    // Escape behaviour can't be overriden: navigation primitives need to remain
    // predictable.
    if (keyEvent->key() == Qt::Key_Escape) return false;

    // we need to handle this in the event filter so that actions bound with, for instance, CTRL-X,
    // get properly executed instead of letting the input consume the CTRL-X key sequence as it normally
    // would (used for cutting text)
    // Note: bound actions always take precedence over other kinds of input handling
    if (AbstractAction *action = m_navigation.findBoundAction(keyEvent)) {
      m_navigation.executeAction(action);
      return true;
    }

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

  if (auto state = m_navigation.topState()) {
    if (state->sender->onBackspace()) return;
  }

  m_navigation.popCurrentView();
}

GlobalHeader::GlobalHeader(NavigationController &controller) : m_navigation(controller) {
  setupUI();
  connect(&m_navigation, &NavigationController::searchTextSelected, m_input, &SearchBar::selectAll);
  connect(&m_navigation, &NavigationController::currentViewChanged, this, [this]() {
    if (auto state = m_navigation.topState()) {
      bool needsBackButton = m_navigation.viewStackSize() > 1 && state->sender->showBackButton();

      m_backButton->setVisible(needsBackButton);
      m_backButtonSpacer->setVisible(needsBackButton);
    }
  });
  connect(&m_navigation, &NavigationController::searchAccessoryChanged, this, &GlobalHeader::setAccessory);
  connect(&m_navigation, &NavigationController::searchAccessoryVisiblityChanged, m_accessoryContainer,
          &QStackedWidget::setVisible);
  connect(&m_navigation, &NavigationController::searchAccessoryCleared, this, &GlobalHeader::clearAccessory);
  connect(&m_navigation, &NavigationController::searchTextChanged, m_input, [this](const QString &text) {
    if (m_input->text() == text) return; // prevents losing cursor position during editing
    m_input->setText(text);
  });
  connect(&m_navigation, &NavigationController::searchPlaceholderTextChanged, m_input,
          &SearchBar::setPlaceholderText);
  connect(m_input, &SearchBar::pop, this, &GlobalHeader::handleSearchPop);
  connect(m_backButton, &IconButton::clicked, &m_navigation, &NavigationController::popCurrentView);
  connect(m_input, &SearchBar::textEdited, this, &GlobalHeader::handleTextEdited);
  connect(&m_navigation, &NavigationController::loadingChanged, m_loadingBar,
          &HorizontalLoadingBar::setStarted);
}
