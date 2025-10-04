#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "extend/action-model.hpp"
#include "theme.hpp"
#include "ui/image/builtin-icon-loader.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qwidget.h>
#include <unordered_map>

// clang-format off
static std::unordered_map<QString, QString> keyToIcon = {
	{"ctrl", ":icons/chevron-up.svg"},
	{"shift", ":icons/keyboard-shift.svg"},
	{"return", ":icons/enter-key.svg"},
	{"cmd", ":icons/command-symbol.svg"},
	{"opt", ":icons/option-symbol.svg"}
};
// clang-format on

void KeyboardShortcutIndicatorWidget::setBackgroundColor(ColorLike color) { m_backgroundColor = color; }
void KeyboardShortcutIndicatorWidget::setColor(ColorLike color) { m_color = color; }

void KeyboardShortcutIndicatorWidget::drawKey(const QString &key, QRect rect, OmniPainter &painter) {
  int padding = height() * 0.2;
  auto &theme = ThemeService::instance().theme();
  QPainterPath path;

  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setThemeBrush(m_backgroundColor);
  painter.setPen(Qt::NoPen);
  path.addRoundedRect(rect, 6, 6);

  painter.setClipPath(path);
  painter.drawPath(path);

  QRect contentRect(rect.x() + padding, rect.y() + padding, rect.width() - padding * 2,
                    rect.height() - padding * 2);

  if (auto it = keyToIcon.find(key); it != keyToIcon.end()) {
    BuiltinIconLoader loader(it->second);
    QPixmap pix = loader.renderSync({.size = contentRect.size(),
                                     .devicePixelRatio = qApp->devicePixelRatio(),
                                     .fill = SemanticColor::TextPrimary});

    painter.drawPixmap(contentRect, pix);
  } else {
    painter.setThemePen(m_color);
    painter.drawText(contentRect, Qt::AlignCenter, key.toUpper());
  }
}

void KeyboardShortcutIndicatorWidget::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QRect rect{0, 0, height(), height()};

  for (const auto &mod : _shortcutModel.modifiers) {
    drawKey(mod, rect, painter);
    rect.moveLeft(rect.left() + height() + _hspacing);
  }

  if (!_shortcutModel.key.isEmpty()) { drawKey(_shortcutModel.key, rect, painter); }
}

QSize KeyboardShortcutIndicatorWidget::sizeHint() const {
  int count = _shortcutModel.modifiers.size() + 1;
  int width = count * _boxSize + ((count - 1) * _hspacing);

  return {width, _boxSize};
}

void KeyboardShortcutIndicatorWidget::setShortcut(const KeyboardShortcutModel &model) {
  _shortcutModel = model;
  updateGeometry();
  update();
}

KeyboardShortcutIndicatorWidget::KeyboardShortcutIndicatorWidget(QWidget *parent) : QWidget(parent) {}
