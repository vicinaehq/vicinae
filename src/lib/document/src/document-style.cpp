#include <QFontDatabase>
#include <QGuiApplication>
#include <QPalette>
#include <algorithm>
#include "document-style.hpp"

namespace vicinae::document {

DocumentStyle::DocumentStyle(QObject *parent) : QObject(parent) {
  const auto palette = QGuiApplication::palette();
  const auto font = QGuiApplication::font();
  const auto withAlpha = [](QColor color, qreal alpha) {
    color.setAlphaF(alpha);
    return color;
  };
  fontFamily = font.family();
  monoFontFamily = QFontDatabase::systemFont(QFontDatabase::FixedFont).family();
  regularFontSize = font.pointSizeF();
  smallerFontSize = std::max(1.0, font.pointSizeF() - 1);
  isDark = palette.color(QPalette::Window).lightnessF() < 0.5;
  foreground = palette.color(QPalette::Text);
  textMuted = palette.color(QPalette::PlaceholderText);
  divider = palette.color(QPalette::Mid);
  secondaryBackground = palette.color(QPalette::Base);
  textSelectionBg = palette.color(QPalette::Highlight);
  textSelectionFg = palette.color(QPalette::HighlightedText);
  accent = palette.color(QPalette::Highlight);
  linkColor = palette.color(QPalette::Link);
  codeBackground = palette.color(QPalette::Base);
  codeBorder = palette.color(QPalette::Mid);
  inlineCodeBackground = withAlpha(foreground.value(), 0.08);
  tableBorder = withAlpha(foreground.value(), 0.1);
  tableHeaderBackground = withAlpha(foreground.value(), 0.06);
  scrollBarColor = palette.color(QPalette::Mid);
  infoColor = QColor("#5794db");
  warningColor = QColor("#d59b31");
  dangerColor = QColor("#d85a59");
  successColor = QColor("#59a66f");
  keywordColor = QColor("#b079d4");
  functionColor = QColor("#5794db");
  variableColor = QColor("#d85a59");
  builtinColor = QColor("#54a9ac");
  numberColor = QColor("#d59b31");
  stringColor = QColor("#59a66f");
  commentColor = textMuted.value();
}

DocumentStyle *DocumentStyle::defaults() {
  static auto *style = new DocumentStyle(QGuiApplication::instance());
  return style;
}

} // namespace vicinae::document
