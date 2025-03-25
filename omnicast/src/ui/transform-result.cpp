#include "ui/transform-result.hpp"
#include "omnicast.hpp"
#include "theme.hpp"
#include "ui/ellided-label.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include <qboxlayout.h>
#include <qfont.h>
#include <qlabel.h>
#include <qregion.h>
#include <qtextformat.h>
#include <qwidget.h>

int TransformResult::availableHeight() const {
  auto margins = contentsMargins();

  return height() - margins.top() - margins.bottom();
}

QWidget *TransformResult::createVContainer(QWidget *left, QWidget *right) const {
  auto container = new QWidget;
  auto layout = new QVBoxLayout();

  layout->setSpacing(10);
  layout->addWidget(left, 1, Qt::AlignCenter);
  layout->addWidget(right, 1, Qt::AlignBottom | Qt::AlignCenter);
  container->setLayout(layout);

  return container;
}

QLabel *TransformResult::createLabel(const QString &text, QWidget *parent) const {
  auto label = new EllidedLabel(text, parent);

  // Not pretty, but programatically setting the font does not work in this case, probably getting overwritten
  // somewhere...
  label->setStyleSheet("font-size: 16pt; font-weight: bold;");

  return label;
}

QSize TransformResult::sizeHint() const {
  QSize size{0, 0};

  if (parentWidget()) { size.setWidth(parentWidget()->width()); }

  if (_base) { size.setHeight(_base->sizeHint().height()); }

  if (_result) {
    int hint = _result->sizeHint().height();

    if (hint > size.height()) size.setHeight(hint);
  }

  auto margins = contentsMargins();

  return {size.width(), size.height() + margins.top() + margins.bottom()};
}

void TransformResult::selectionChanged(bool value) {
  SelectableOmniListWidget::selectionChanged(value);
  setDividerVisible(value);
}

void TransformResult::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  SelectableOmniListWidget::paintEvent(event);
  QPainter painter(this);
  int midW = width() / 2;
  int midH = height() / 2;
  auto margins = contentsMargins();

  painter.setPen(Qt::NoPen);
  painter.setBrush(theme.colors.subtext);

  _base->setFixedSize({midW, availableHeight()});
  _base->move(0, margins.top());

  _result->setFixedSize({midW, availableHeight()});
  _result->move(midW, margins.top());

  if (selected() || hovered()) {
    painter.drawRect(midW, 0, 1, midH - _arrowMid.height());
    painter.drawRect(midW, midH + _arrowMid.width(), 1, midH - _arrowMid.height());
  }

  painter.drawPixmap(midW - _arrowMid.width(), midH - _arrowMid.height(), _arrowIcon);
}

void TransformResult::setDividerVisible(bool visible) {
  _isDividerVisible = visible;
  update();
}

void TransformResult::setDividerColor(QColor color) {
  _dividerColor = color;
  update();
}

void TransformResult::setBase(const QString &text, const QString &chip) { setBase(createLabel(text), chip); }

void TransformResult::setResult(const QString &text, const QString &chip) {
  setResult(createLabel(text), chip);
}

void TransformResult::setBase(QWidget *widget, const QString &chip) {
  if (_base) { _base->deleteLater(); }

  _base = createVContainer(widget, new QLabel(chip));
  _base->setParent(this);
  update();
}

void TransformResult::setResult(QWidget *widget, const QString &chip) {
  if (_result) { _result->deleteLater(); }

  _result = createVContainer(widget, new QLabel(chip));
  _result->setParent(this);
  update();
}

TransformResult::TransformResult()
    : _isDividerVisible(true), _dividerColor("#666666"), _base(nullptr), _result(nullptr) {
  auto icon = QIcon(":icons/arrow-right.svg");

  setContentsMargins(10, 10, 10, 10);
  _arrowIcon = icon.pixmap(32, 32).scaledToWidth(32, Qt::SmoothTransformation);
  _arrowMid = {_arrowIcon.width() / 2, _arrowIcon.height() / 2};
}
