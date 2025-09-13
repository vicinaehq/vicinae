#include "dialog.hpp"
#include "service-registry.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include "services/config/config-service.hpp"

DialogContentWidget::DialogContentWidget(QWidget *parent) : QWidget(parent) {}

void DialogWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  QColor finalColor = painter.resolveColor(SemanticColor::MainBackground);
  auto config = ServiceRegistry::instance()->config()->value();
  painter.setBrush(finalColor);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(rect(), config.window.rounding, config.window.rounding);

  QWidget::paintEvent(event);
}

void DialogWidget::setContent(DialogContentWidget *content) {
  if (auto item = _layout->takeAt(0)) { item->widget()->deleteLater(); }

  setFocusProxy(content);
  connect(content, &DialogContentWidget::closeRequested, this, &DialogWidget::hide);

  _content = content;
  _layout->addWidget(content, 0, Qt::AlignCenter);
}

void DialogWidget::showDialog() {
  auto parent = parentWidget();

  if (!parent) {
    qWarning() << "Dialog has no parent widget, can't position";
    return;
  }

  setFixedSize(parent->size());
  move(0, 0);
  raise();
  show();
}

void DialogWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    if (_content) _content->interrupted();
    hide();
    return;
  };
  return QWidget::keyPressEvent(event);
}

DialogWidget::DialogWidget(QWidget *parent) : QWidget(parent), _layout(new QVBoxLayout), _content(nullptr) {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(0);
  setLayout(_layout);
}
