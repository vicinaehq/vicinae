#include "scroll-bar.hpp"
#include "theme.hpp"
#include <qscrollbar.h>
#include <qwidget.h>

OmniScrollBar::OmniScrollBar(QWidget *parent) : QScrollBar(parent) {
  setStyleSheet("background: transparent;");
  setAttribute(Qt::WA_Hover, true);

  dismissTimer.setSingleShot(true);
  dismissTimer.setInterval(500);

  connect(this, &QScrollBar::valueChanged, this, &OmniScrollBar::handleValueChanged);
  connect(&dismissTimer, &QTimer::timeout, this, [this]() { setSliderVisibility(false); });
}

void OmniScrollBar::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  auto &theme = ThemeService::instance().theme();

  if (!isSliderShown) return;

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // Get scrollbar handle geometry
  QStyleOptionSlider opt;
  opt.initFrom(this);
  opt.orientation = orientation();
  opt.minimum = minimum();
  opt.maximum = maximum();
  opt.sliderPosition = value();
  opt.sliderValue = value();
  opt.singleStep = singleStep();
  opt.pageStep = pageStep();

  QRect handleRect = style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, this);

  painter.setBrush(theme.resolveTint(SemanticColor::MainSelectedBackground));
  painter.setPen(Qt::NoPen);

  // Rounded scrollbar
  int radius = 4;
  if (orientation() == Qt::Horizontal) {
    // handleRect.setHeight(8); // Ensure correct thickness
  } else {
    handleRect.setWidth(8);

    auto xAdjust = round((width() - handleRect.width()) / (double)2);

    handleRect = handleRect.adjusted(xAdjust, 0, xAdjust, 0);
  }
  painter.drawRoundedRect(handleRect, radius, radius);
}

void OmniScrollBar::setSliderVisibility(bool visible) {
  isSliderShown = visible;
  update();
}

void OmniScrollBar::enterEvent(QEnterEvent *event) {
  isHovered = true;
  setSliderVisibility(true);
}

void OmniScrollBar::leaveEvent(QEvent *event) {
  isHovered = false;
  setSliderVisibility(false);
}

void OmniScrollBar::handleValueChanged(int value) {
  if (!isHovered) {
    isSliderShown = true;
    dismissTimer.start();
  }
}
