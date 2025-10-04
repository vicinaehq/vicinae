#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "extend/action-model.hpp"
#include "keyboard/keyboard.hpp"
#include "theme.hpp"
#include "ui/image/builtin-icon-loader.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qwidget.h>
#include <unordered_map>

// clang-format off
static std::unordered_map<Qt::KeyboardModifier, QString> keyToIcon = {
	{Qt::ControlModifier, ":icons/chevron-up.svg"},
	{Qt::ShiftModifier, ":icons/keyboard-shift.svg"},
	{Qt::MetaModifier, ":icons/command-symbol.svg"},
	{Qt::AltModifier, ":icons/option-symbol.svg"}
};

// clang-format on

void KeyboardShortcutIndicatorWidget::setBackgroundColor(ColorLike color) { m_backgroundColor = color; }
void KeyboardShortcutIndicatorWidget::setColor(ColorLike color) { m_color = color; }

void KeyboardShortcutIndicatorWidget::drawKey(Qt::Key key, QRect rect, OmniPainter &painter) {
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

  painter.setThemePen(m_color);

  if (key == Qt::Key_Return) {
    painter.drawText(contentRect, Qt::AlignCenter, "⏎");
  } else {

    if (auto keyStr = Keyboard::stringForKey(key)) {
      painter.drawText(contentRect, Qt::AlignCenter, keyStr->toUpper());
    }
  }
}

void KeyboardShortcutIndicatorWidget::drawModifier(Qt::KeyboardModifier mod, QRect rect,
                                                   OmniPainter &painter) {
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

  if (auto it = keyToIcon.find(mod); it != keyToIcon.end()) {
    BuiltinIconLoader loader(it->second);
    QPixmap pix = loader.renderSync(
        {.size = contentRect.size(), .devicePixelRatio = qApp->devicePixelRatio(), .fill = m_color});

    painter.drawPixmap(contentRect, pix);
  }
}

void KeyboardShortcutIndicatorWidget::paintEvent(QPaintEvent *event) {
  QRect rect{0, 0, height(), height()};
  OmniPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);

  for (const auto &mod : m_shortcut.modList()) {
    drawModifier(mod, rect, painter);
    rect.moveLeft(rect.left() + height() + _hspacing);
  }

  drawKey(m_shortcut.key(), rect, painter);
}

QSize KeyboardShortcutIndicatorWidget::sizeHint() const {
  int count = m_shortcut.modList().size() + 1;
  int width = count * _boxSize + ((count - 1) * _hspacing);

  return {width, _boxSize};
}

void KeyboardShortcutIndicatorWidget::setShortcut(const Keyboard::Shortcut &model) {
  m_shortcut = model;
  updateGeometry();
  update();
}

KeyboardShortcutIndicatorWidget::KeyboardShortcutIndicatorWidget(QWidget *parent) : QWidget(parent) {}
