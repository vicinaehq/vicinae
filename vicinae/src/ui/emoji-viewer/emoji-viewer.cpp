#include "ui/emoji-viewer/emoji-viewer.hpp"
#include "service-registry.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "font-service.hpp"

void EmojiViewer::setEmoji(const QString &emoji) {
  _emoji = emoji;
  update();
}

void EmojiViewer::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  if (_scaleHeight != -1) { _pointSize = height() * _scaleHeight; }
}

void EmojiViewer::setHeightScale(double factor) { _scaleHeight = factor; }

void EmojiViewer::setPointSize(int size) { _pointSize = size; }

void EmojiViewer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  QFont font = ServiceRegistry::instance()->fontService()->emojiFont();

  font.setPointSize(_pointSize);
  painter.setFont(font);
  painter.drawText(rect(), Qt::AlignCenter, _emoji);
}

void EmojiViewer::setAlignment(Qt::AlignmentFlag align) { _align = align; }

EmojiViewer::EmojiViewer(const QString &emoji)
    : _emoji(emoji), _pointSize(10), _scaleHeight(-1), _align(Qt::AlignTop) {}
