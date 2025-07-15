#include "launcher-window.hpp"
#include "header.hpp"
#include "navigation-controller.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "root-command.hpp"

LauncherWindow::LauncherWindow() : m_header(new GlobalHeader(m_navigation)) {
  setupUI();
  m_navigation.pushView(new RootSearchView);
}

void LauncherWindow::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(Omnicast::WINDOW_SIZE);
  setCentralWidget(createWidget());
  connect(&m_navigation, &NavigationController::currentViewChanged, this, &LauncherWindow::handleViewChange);
}

void LauncherWindow::handleViewChange(const NavigationController::ViewState &state) {
  if (auto current = m_currentViewWrapper->widget(0)) { m_currentViewWrapper->removeWidget(current); }

  m_currentViewWrapper->addWidget(state.sender);
  m_currentViewWrapper->setCurrentWidget(state.sender);
}

bool LauncherWindow::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    switch (keyEvent->key()) {
    case Qt::Key_Escape: {
      if (m_navigation.viewStackSize() == 1) {
        if (m_navigation.searchText().isEmpty()) {
          close();
          return true;
        }

        m_navigation.clearSearchText();
        return true;
      }

      m_navigation.popCurrentView();
      return true;
    }
    default:
      break;
    }
  }

  return QMainWindow::event(event);
}

void LauncherWindow::paintEvent(QPaintEvent *event) {
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  int borderWidth = 2;
  QColor finalBgColor = theme.colors.mainBackground;
  QPainter painter(this);

  finalBgColor.setAlphaF(config.window.opacity);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), config.window.rounding, config.window.rounding);

  painter.setClipPath(path);

  painter.fillPath(path, finalBgColor);

  QPen pen(theme.colors.border, borderWidth);
  painter.setPen(pen);

  painter.drawPath(path);
}

QWidget *LauncherWindow::createWidget() const {
  auto layout = new QVBoxLayout;
  auto widget = new QWidget;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_header);
  layout->addWidget(m_currentViewWrapper, 1);
  layout->addWidget(m_bar, 1);
  widget->setLayout(layout);

  return widget;
}
