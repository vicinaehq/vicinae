#include "launcher-window.hpp"
#include "header.hpp"
#include "navigation-controller.hpp"
#include <qboxlayout.h>
#include <qwidget.h>

LauncherWindow::LauncherWindow() : m_header(new GlobalHeader(m_navigation)) { setupUI(); }

void LauncherWindow::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(Omnicast::WINDOW_SIZE);
  setCentralWidget(createWidget());
  connect(&m_navigation, &NavigationController::currentViewChanged, this, &LauncherWindow::handleViewChange);
}

void LauncherWindow::handleViewChange(const NavigationController::ViewState &state) {
  if (auto current = m_currentViewWrapper->widget(0)) { m_currentViewWrapper->removeWidget(current); }

  m_currentViewWrapper->addWidget(state.sender.get());
  m_currentViewWrapper->setCurrentWidget(state.sender.get());
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
