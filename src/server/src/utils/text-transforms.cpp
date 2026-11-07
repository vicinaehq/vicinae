#include "utils/text-transforms.hpp"
#include <QRegularExpression>
#include <algorithm>
#include <cmath>

namespace text_transforms {

QString toUppercase(const QString &text) { return text.toUpper(); }

QString toLowercase(const QString &text) { return text.toLower(); }

QString toTitleCase(const QString &text) {
  QString result;
  result.reserve(text.size());
  bool newWord = true;

  for (const auto ch : text) {
    if (ch.isSpace() || ch == '_' || ch == '-') {
      result.append(ch);
      newWord = true;
    } else if (newWord) {
      result.append(ch.toUpper());
      newWord = false;
    } else {
      result.append(ch.toLower());
    }
  }

  return result;
}

QString toSlug(const QString &text) {
  QString slug;
  slug.reserve(text.size());
  bool lastWasDash = false;

  for (const auto ch : text.trimmed()) {
    if (ch.isLetterOrNumber()) {
      slug.append(ch.toLower());
      lastWasDash = false;
    } else if (!lastWasDash && !slug.isEmpty()) {
      slug.append('-');
      lastWasDash = true;
    }
  }

  if (slug.endsWith('-')) slug.chop(1);
  return slug;
}

QString trim(const QString &text) { return text.trimmed(); }

std::optional<QColor> parseColor(const QString &text) {
  auto t = text.trimmed();
  if (t.size() < 3 || t.size() > 50) return std::nullopt;

  if (t.startsWith('#')) {
    QColor col(t);
    if (col.isValid()) return col;
    return std::nullopt;
  }

  static const QRegularExpression rgbRegex(
      QStringLiteral(
          R"(^rgba?\s*\(\s*([0-9.]+)(%?)\s*[, ]\s*([0-9.]+)(%?)\s*[, ]\s*([0-9.]+)(%?)(?:\s*[,/]\s*([0-9.]+)(%?))?\s*\)$)"),
      QRegularExpression::CaseInsensitiveOption);
  auto rgbMatch = rgbRegex.match(t);
  if (rgbMatch.hasMatch()) {
    auto parseVal = [](const QString &val, const QString &pct) -> double {
      double v = val.toDouble();
      if (!pct.isEmpty()) v = (v / 100.0) * 255.0;
      return std::clamp(v, 0.0, 255.0);
    };
    int r = static_cast<int>(std::round(parseVal(rgbMatch.captured(1), rgbMatch.captured(2))));
    int g = static_cast<int>(std::round(parseVal(rgbMatch.captured(3), rgbMatch.captured(4))));
    int b = static_cast<int>(std::round(parseVal(rgbMatch.captured(5), rgbMatch.captured(6))));
    int a = 255;
    if (rgbMatch.capturedLength(7) > 0) {
      double alphaVal = rgbMatch.captured(7).toDouble();
      if (!rgbMatch.captured(8).isEmpty()) {
        alphaVal /= 100.0;
      } else if (alphaVal > 1.0) {
        alphaVal /= 255.0;
      }
      a = static_cast<int>(std::round(std::clamp(alphaVal, 0.0, 1.0) * 255.0));
    }
    return QColor(r, g, b, a);
  }

  static const QRegularExpression hslRegex(
      QStringLiteral(
          R"(^hsla?\s*\(\s*([0-9.]+)(?:deg)?\s*[, ]\s*([0-9.]+)%\s*[, ]\s*([0-9.]+)%(?:\s*[,/]\s*([0-9.]+)(%?))?\s*\)$)"),
      QRegularExpression::CaseInsensitiveOption);
  auto hslMatch = hslRegex.match(t);
  if (hslMatch.hasMatch()) {
    double h = hslMatch.captured(1).toDouble();
    double s = hslMatch.captured(2).toDouble() / 100.0;
    double l = hslMatch.captured(3).toDouble() / 100.0;
    double a = 1.0;
    if (hslMatch.capturedLength(4) > 0) {
      a = hslMatch.captured(4).toDouble();
      if (!hslMatch.captured(5).isEmpty())
        a /= 100.0;
      else if (a > 1.0)
        a /= 255.0;
      a = std::clamp(a, 0.0, 1.0);
    }
    int hue = static_cast<int>(std::round(h)) % 360;
    if (hue < 0) hue += 360;
    return QColor::fromHslF(hue / 360.0, std::clamp(s, 0.0, 1.0), std::clamp(l, 0.0, 1.0), a);
  }

  QColor col(t);
  if (col.isValid()) return col;

  return std::nullopt;
}

QString formatHex(const QColor &color) {
  return color.name(color.alpha() < 255 ? QColor::HexArgb : QColor::HexRgb).toLower();
}

QString formatRgb(const QColor &color) {
  if (color.alpha() < 255) {
    return QStringLiteral("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(QString::number(color.alphaF(), 'f', 2));
  }
  return QStringLiteral("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
}

QString formatHsl(const QColor &color) {
  int h = color.hslHue() >= 0 ? color.hslHue() : 0;
  int s = static_cast<int>(std::round(color.hslSaturationF() * 100));
  int l = static_cast<int>(std::round(color.lightnessF() * 100));

  if (color.alpha() < 255) {
    return QStringLiteral("hsla(%1, %2%, %3%, %4)")
        .arg(h)
        .arg(s)
        .arg(l)
        .arg(QString::number(color.alphaF(), 'f', 2));
  }
  return QStringLiteral("hsl(%1, %2%, %3%)").arg(h).arg(s).arg(l);
}

} // namespace text_transforms
