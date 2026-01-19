#include "ui/keyboard-shortcut-indicator/keyboard-shortcut-indicator.hpp"
#include "common/types.hpp"
#include "keyboard/keyboard.hpp"
#include "theme.hpp"
#include "ui/image/builtin-icon-loader.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qwidget.h>
#include <unordered_map>

struct KeyText {
  QString text;
};
struct KeyIcon {
  QString icon;
};

using KeySymbol = std::variant<KeyIcon, KeyText>;

// clang-format off
static std::unordered_map<Qt::Key, KeySymbol> keyRepresentationOverloads = {
    {Qt::Key_Meta, KeyIcon("command-symbol")},
	{Qt::Key_Alt, KeyIcon("option-symbol")},
    {Qt::Key_Control, KeyIcon("chevron-up")},
	{Qt::Key_Shift, KeyIcon("keyboard-shift")},
    {Qt::Key_Return, KeyIcon("enter-key")},
    {Qt::Key_Left, KeyIcon("arrow-left")},
    {Qt::Key_Right, KeyIcon("arrow-right")},
    {Qt::Key_Up, KeyIcon("arrow-up")},
    {Qt::Key_Down, KeyIcon("arrow-down")},
    {Qt::Key_Backspace, KeyIcon("arrow-counter-clockwise")},
    {Qt::Key_Tab, KeyIcon("tab-key")},
    {Qt::Key_Space, KeyIcon("space-key")},
};
// clang-format on

void KeyboardShortcutIndicatorWidget::setBackgroundColor(ColorLike color) { m_backgroundColor = color; }
void KeyboardShortcutIndicatorWidget::setColor(ColorLike color) { m_color = color; }

QSize KeyboardShortcutIndicatorWidget::sizeForKey(Qt::Key key) const {
  auto sizeForString = [&](const QString &text) {
    int advance = fontMetrics().horizontalAdvance(text);
    int padding = _boxSize * 0.2;
    int availableHorizontalWidth = _boxSize - padding * 2;

    if (advance < availableHorizontalWidth) { return QSize(_boxSize, _boxSize); }
    return QSize(advance + padding * 2, _boxSize);
  };

  if (auto it = keyRepresentationOverloads.find(key); it != keyRepresentationOverloads.end()) {
    const auto visitor = overloads{[&](const KeyIcon &icon) { return QSize(_boxSize, _boxSize); },
                                   [&](const KeyText &sym) { return sizeForString(sym.text); }};

    return std::visit(visitor, it->second);
  }

  if (auto str = Keyboard::stringForKey(key)) { return sizeForString(str.value()); }

  return QSize();
}

void KeyboardShortcutIndicatorWidget::drawKey(Qt::Key key, QRect rect, OmniPainter &painter) {
  int padding = _boxSize * 0.2;
  auto &theme = ThemeService::instance().theme();
  QPainterPath path;

  path.addRoundedRect(rect, 6, 6);
  painter.setThemeBrush(m_backgroundColor);
  painter.setPen(Qt::NoPen);
  painter.setClipPath(path);
  painter.drawPath(path);

  QRect contentRect(rect.x() + padding, rect.y() + padding, rect.width() - padding * 2,
                    rect.height() - padding * 2);

  painter.setThemePen(m_color);

  auto drawText = [&](const QString &text) { painter.drawText(contentRect, Qt::AlignCenter, text); };

  const auto visitor = overloads{
      [&](const KeyText key) { return drawText(key.text); },
      [&](const KeyIcon icon) {
        BuiltinIconLoader loader(QString(":icons/%1.svg").arg(icon.icon));
        QPixmap pix = loader.renderSync(
            {.size = contentRect.size(), .devicePixelRatio = qApp->devicePixelRatio(), .fill = m_color});
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.drawPixmap(contentRect, pix);
      }};

  if (auto it = keyRepresentationOverloads.find(key); it != keyRepresentationOverloads.end()) {
    std::visit(visitor, it->second);
  } else {
    if (auto str = Keyboard::stringForKey(key)) { drawText(str.value().toUpper()); }
  }
}

void KeyboardShortcutIndicatorWidget::paintEvent(QPaintEvent *event) {
  QRect rect{0, 0, height(), height()};
  OmniPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  for (const auto &key : m_shortcut.allKeys()) {
    auto size = sizeForKey(key);
    rect.setWidth(size.width());
    drawKey(key, rect, painter);
    rect.moveLeft(rect.left() + height() + _hspacing);
  }

  // if (m_shortcut.isValidKey()) { drawKey(m_shortcut.key(), rect, painter); }
}

QSize KeyboardShortcutIndicatorWidget::sizeHint() const {
  int count = m_shortcut.modList().size();
  int width = count * _boxSize + ((count - 1) * _hspacing);

  if (m_shortcut.isValidKey()) { width += sizeForKey(m_shortcut.key()).width() + _hspacing; }

  return {width, _boxSize};
}

void KeyboardShortcutIndicatorWidget::setShortcut(const Keyboard::Shortcut &model) {
  m_shortcut = model;
  updateGeometry();
  update();
}

KeyboardShortcutIndicatorWidget::KeyboardShortcutIndicatorWidget(QWidget *parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
