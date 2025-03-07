#include "ui/keyboard-shortcut-indicator.hpp"
#include "builtin_icon.hpp"
#include "extend/action-model.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qwidget.h>
#include <unordered_map>

// clang-format off
static std::unordered_map<QString, QString> keyToIcon = {
	{"ctrl", ":icons/chevron-up.svg"},
	{"shift", ":icons/keyboard-shift.svg"},
	{"return", ":icons/enter-key.svg"}
};
// clang-format on

void KeyboardShortcutIndicatorWidget::setBackgroundColor(QColor color) { _backgroundColor = color; }

void KeyboardShortcutIndicatorWidget::drawKey(const QString &key, QRect rect, QPainter &painter) {
  int padding = height() * 0.2;

  painter.setPen(_backgroundColor);
  painter.setBrush(QBrush(_backgroundColor));
  painter.drawRoundedRect(rect, 6, 6);

  QRect contentRect(rect.x() + padding, rect.y() + padding, rect.width() - padding * 2,
                    rect.height() - padding * 2);

  if (auto it = keyToIcon.find(key); it != keyToIcon.end()) {
    auto controlIcon = BuiltinIconService::loadTinted(it->second, "#999999");

    qDebug() << "loading pixmap" << it->second;

    painter.drawPixmap(contentRect,
                       controlIcon.scaled(contentRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  } else {
    painter.setPen("#999999");
    painter.drawText(contentRect, Qt::AlignCenter, _shortcutModel.key);
  }
}

void KeyboardShortcutIndicatorWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QRect rect{0, 0, height(), height()};

  for (const auto &mod : _shortcutModel.modifiers) {
    qDebug() << "modifier" << mod;
    drawKey(mod, rect, painter);
    rect.moveLeft(rect.left() + height() + _hspacing);
  }

  drawKey(_shortcutModel.key, rect, painter);
}

QSize KeyboardShortcutIndicatorWidget::sizeHint() const {
  int count = _shortcutModel.modifiers.size() + 1;
  int width = count * _boxSize + ((count - 1) * _hspacing);

  qDebug() << "size hint returned" << count;

  return {width, _boxSize};
}

void KeyboardShortcutIndicatorWidget::setShortcut(const KeyboardShortcutModel &model) {
  qDebug() << "set shortcut to" << model.key << "with" << model.modifiers;
  _shortcutModel = model;
  updateGeometry();
  update();
}

KeyboardShortcutIndicatorWidget::KeyboardShortcutIndicatorWidget(QWidget *parent)
    : QWidget(parent), _backgroundColor("#222222") {}
