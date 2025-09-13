#include "ui/loading-bar/horizontal-loading-bar.hpp"
#include "theme.hpp"
#include <chrono>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qwidget.h>

static const int DEFAULT_BAR_WIDTH = 50;
static const int DEFAULT_STEP = 10;
static const std::chrono::milliseconds DEFAULT_TICK(10);

void HorizontalLoadingBar::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  QPainter painter(this);

  painter.setPen(Qt::NoPen);
  painter.setBrush(theme.resolveTint(SemanticColor::Border));
  painter.drawRect(rect());

  if (_isAnimationStarted) {
    painter.setBrush(theme.resolveTint(SemanticColor::TextSecondary));
    painter.drawRect(QRect(_position, 0, _barWidth, height()));
  }
}

void HorizontalLoadingBar::updateAnimation() {
  _position += _positionStep;

  if (_position > width()) { _position = -_barWidth; }
  update();
}

void HorizontalLoadingBar::setPositionStep(int step) { _positionStep = step; }

void HorizontalLoadingBar::setTick(std::chrono::milliseconds ms) { _tick = ms; }

void HorizontalLoadingBar::start() {
  _isAnimationStarted = true;

  if (!_timer->isActive()) { _timer->start(_tick); }

  update();
}

void HorizontalLoadingBar::setStarted(bool started) {
  m_started = started;
  m_loadingDebounce.start();
}

void HorizontalLoadingBar::stop() {
  _isAnimationStarted = false;
  _timer->stop();
  _position = 0;
  update();
}

void HorizontalLoadingBar::setBarWidth(int width) {
  _barWidth = width;
  update();
}

HorizontalLoadingBar::HorizontalLoadingBar(QWidget *parent)
    : QWidget(parent), _isAnimationStarted(false), _timer(new QTimer(this)), _barWidth(DEFAULT_BAR_WIDTH),
      _tick(DEFAULT_TICK), _positionStep(DEFAULT_STEP) {
  using namespace std::chrono_literals;

  setFixedHeight(1);
  m_loadingDebounce.setInterval(0ms);
  m_loadingDebounce.setSingleShot(true);

  connect(&m_loadingDebounce, &QTimer::timeout, this, [this]() {
    if (m_started) {
      start();
    } else {
      stop();
    }
  });

  connect(_timer, &QTimer::timeout, this, &HorizontalLoadingBar::updateAnimation);
}
