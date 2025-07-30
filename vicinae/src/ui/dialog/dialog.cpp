#include "dialog.hpp"
#include "ui/omni-painter.hpp"

DialogContentWidget::DialogContentWidget(QWidget *parent) : QWidget(parent) {}

void DialogWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  QColor finalColor = painter.resolveColor(SemanticColor::MainBackground);

  finalColor.setAlphaF(0.5);

  painter.setBrush(finalColor);
  painter.setPen(Qt::NoPen);
  painter.drawRect(rect());

  QWidget::paintEvent(event);
}

void DialogWidget::setContent(DialogContentWidget *content) {
  if (auto item = _layout->takeAt(0)) { item->widget()->deleteLater(); }

  setFocusProxy(content);
  connect(content, &DialogContentWidget::closeRequested, this, &DialogContentWidget::close);

  _layout->addWidget(content, 0, Qt::AlignCenter);
}

void DialogWidget::showDialog() {
  if (auto w = parentWidget()) {
    setGeometry(w->geometry());
    show();
  }
}

DialogWidget::DialogWidget(QWidget *parent) : QWidget(parent), _layout(new QVBoxLayout), _content(nullptr) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);

  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(0);
  setLayout(_layout);
}
