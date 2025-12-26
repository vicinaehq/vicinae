#include <QRegularExpression>
#include <QStringList>
#include <cmath>
#include "color-formatter.hpp"

QString ColorFormatter::format(const QColor &color, ColorFormat format) {
  if (!color.isValid()) { return QString(); }

  switch (format) {
  case RGB:
    return formatRgb(color);
  case RGBA:
    return formatRgba(color);
  case RGBA_PERCENTAGE:
    return formatRgbaPercentage(color);
  case HEX:
    return formatHex(color);
  case HEX_ALPHA:
    return formatHexAlpha(color);
  case HSL:
    return formatHsl(color);
  case HSL_ALPHA:
    return formatHslAlpha(color);
  case HSV:
    return formatHsv(color);
  case HSV_ALPHA:
    return formatHsvAlpha(color);
  default:
    return QString();
  }
}

QString ColorFormatter::formatName(ColorFormat format) const {
  switch (format) {
  case Invalid:
    return "Invalid";
  case RGB:
    return "RGB";
  case RGBA:
    return "RGBA";
  case RGBA_PERCENTAGE:
    return "RGBA (Percentage)";
  case HEX:
    return "Hex";
  case HEX_ALPHA:
    return "Hex with Alpha";
  case HSL:
    return "HSL";
  case HSL_ALPHA:
    return "HSLA";
  case HSV:
    return "HSV";
  case HSV_ALPHA:
    return "HSVA";
  default:
    return "Unknown";
  }
}

std::expected<ColorFormatter::ParsedColor, ColorFormatter::GenericError>
ColorFormatter::parse(const QString &text) {
  QString cleanText = text.trimmed().toLower();

  if (cleanText.isEmpty()) {
    return std::unexpected(GenericError(GenericError::NotAColorFormat, "Empty string"));
  }

  // Try each format in order of likelihood
  if (auto result = tryParseHex(cleanText)) { return result.value(); }

  if (auto result = tryParseRgb(cleanText)) { return result.value(); }

  if (auto result = tryParseHsl(cleanText)) { return result.value(); }

  if (auto result = tryParseHsv(cleanText)) { return result.value(); }

  return std::unexpected(
      GenericError(GenericError::NotAColorFormat, "Text does not match any known color format"));
}

// Private formatting functions
QString ColorFormatter::formatRgb(const QColor &color) {
  return QString("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
}

QString ColorFormatter::formatRgba(const QColor &color) {
  return QString("rgba(%1, %2, %3, %4)")
      .arg(color.red())
      .arg(color.green())
      .arg(color.blue())
      .arg(color.alphaF(), 0, 'g', 3);
}

QString ColorFormatter::formatRgbaPercentage(const QColor &color) {
  return QString("rgba(%1%, %2%, %3%, %4)")
      .arg(std::round(color.red() / 255.0 * 100))
      .arg(std::round(color.green() / 255.0 * 100))
      .arg(std::round(color.blue() / 255.0 * 100))
      .arg(color.alphaF(), 0, 'g', 3);
}

QString ColorFormatter::formatHex(const QColor &color) {
  return QString("#%1%2%3")
      .arg(color.red(), 2, 16, QChar('0'))
      .arg(color.green(), 2, 16, QChar('0'))
      .arg(color.blue(), 2, 16, QChar('0'));
}

QString ColorFormatter::formatHexAlpha(const QColor &color) {
  return QString("#%1%2%3%4")
      .arg(color.red(), 2, 16, QChar('0'))
      .arg(color.green(), 2, 16, QChar('0'))
      .arg(color.blue(), 2, 16, QChar('0'))
      .arg(color.alpha(), 2, 16, QChar('0'));
}

QString ColorFormatter::formatHsl(const QColor &color) {
  return QString("hsl(%1, %2%, %3%)")
      .arg(color.hslHue() == -1 ? 0 : color.hslHue())
      .arg(std::round(color.hslSaturation() / 255.0 * 100))
      .arg(std::round(color.lightness() / 255.0 * 100));
}

QString ColorFormatter::formatHslAlpha(const QColor &color) {
  return QString("hsla(%1, %2%, %3%, %4)")
      .arg(color.hslHue() == -1 ? 0 : color.hslHue())
      .arg(std::round(color.hslSaturation() / 255.0 * 100))
      .arg(std::round(color.lightness() / 255.0 * 100))
      .arg(color.alphaF(), 0, 'g', 3);
}

QString ColorFormatter::formatHsv(const QColor &color) {
  return QString("hsv(%1, %2%, %3%)")
      .arg(color.hsvHue() == -1 ? 0 : color.hsvHue())
      .arg(std::round(color.hsvSaturation() / 255.0 * 100))
      .arg(std::round(color.value() / 255.0 * 100));
}

QString ColorFormatter::formatHsvAlpha(const QColor &color) {
  return QString("hsva(%1, %2%, %3%, %4)")
      .arg(color.hsvHue() == -1 ? 0 : color.hsvHue())
      .arg(std::round(color.hsvSaturation() / 255.0 * 100))
      .arg(std::round(color.value() / 255.0 * 100))
      .arg(color.alphaF(), 0, 'g', 3);
}

// Private parsing functions
std::optional<ColorFormatter::ParsedColor> ColorFormatter::tryParseHex(const QString &text) {
  // Regex for hex colors: #RGB, #RRGGBB, #RGBA, #RRGGBBAA
  QRegularExpression hexRegex("^#([0-9a-f]{3}|[0-9a-f]{4}|[0-9a-f]{6}|[0-9a-f]{8})$");
  QRegularExpressionMatch match = hexRegex.match(text);

  if (!match.hasMatch()) { return std::nullopt; }

  QString hexValue = match.captured(1);

  try {
    if (hexValue.length() == 3) {
      // #RGB -> #RRGGBB
      QString expanded = QString("%1%1%2%2%3%3").arg(hexValue[0]).arg(hexValue[1]).arg(hexValue[2]);
      QColor color("#" + expanded);

      return ParsedColor{HEX, color};
    }

    if (hexValue.length() == 4) {
      // #RGBA -> #RRGGBBAA
      QString expanded =
          QString("%1%1%2%2%3%3%4%4").arg(hexValue[0]).arg(hexValue[1]).arg(hexValue[2]).arg(hexValue[3]);
      QColor color("#" + expanded.left(6));
      int alpha = hexValue.right(1).toInt(nullptr, 16) * 17;
      color.setAlpha(alpha);
      return ParsedColor{HEX_ALPHA, color};
    }

    if (hexValue.length() == 6) {
      QColor color("#" + hexValue);
      return ParsedColor{HEX, color};
    }

    if (hexValue.length() == 8) {
      QColor color("#" + hexValue.left(6));
      int alpha = hexValue.right(2).toInt(nullptr, 16);

      color.setAlpha(alpha);
      return ParsedColor{HEX_ALPHA, color};
    }
  } catch (...) { return std::nullopt; }

  return std::nullopt;
}

std::vector<ColorFormatter::ColorFormat> ColorFormatter::formats() const {
  return {HEX, HEX_ALPHA, RGB, RGBA, RGBA_PERCENTAGE, HSL, HSL_ALPHA, HSV, HSV_ALPHA};
}

std::optional<ColorFormatter::ParsedColor> ColorFormatter::tryParseRgb(const QString &text) {
  // RGB patterns
  QRegularExpression rgbRegex("^rgba?\\s*\\(\\s*([^)]+)\\s*\\)$");
  QRegularExpressionMatch match = rgbRegex.match(text);

  if (!match.hasMatch()) { return std::nullopt; }

  QString values = match.captured(1);
  QStringList parts = values.split(',');

  bool isRgba = text.startsWith("rgba");
  bool isPercentage = values.contains('%');

  if ((!isRgba && parts.size() != 3) || (isRgba && parts.size() != 4)) { return std::nullopt; }

  try {
    QColor color;

    if (isPercentage) {
      // Parse percentage values
      double r = parsePercentage(parts[0].trimmed());
      double g = parsePercentage(parts[1].trimmed());
      double b = parsePercentage(parts[2].trimmed());

      if (r < 0 || r > 100 || g < 0 || g > 100 || b < 0 || b > 100) { return std::nullopt; }

      color.setRgb(std::round(r * 2.55), std::round(g * 2.55), std::round(b * 2.55));

      if (isRgba) {
        double alpha = parts[3].trimmed().toDouble();
        if (alpha < 0 || alpha > 1) return std::nullopt;
        color.setAlphaF(alpha);
        return ParsedColor{RGBA_PERCENTAGE, color};
      }

      return ParsedColor{RGBA_PERCENTAGE, color};
    } else {
      // Parse integer values
      int r = parts[0].trimmed().toInt();
      int g = parts[1].trimmed().toInt();
      int b = parts[2].trimmed().toInt();

      if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) { return std::nullopt; }

      color.setRgb(r, g, b);

      if (isRgba) {
        double alpha = parts[3].trimmed().toDouble();
        if (alpha < 0 || alpha > 1) return std::nullopt;
        color.setAlphaF(alpha);
        return ParsedColor{RGBA, color};
      }

      return ParsedColor{RGB, color};
    }
  } catch (...) { return std::nullopt; }
}

std::optional<ColorFormatter::ParsedColor> ColorFormatter::tryParseHsl(const QString &text) {
  QRegularExpression hslRegex("^hsla?\\s*\\(\\s*([^)]+)\\s*\\)$");
  QRegularExpressionMatch match = hslRegex.match(text);

  if (!match.hasMatch()) { return std::nullopt; }

  QString values = match.captured(1);
  QStringList parts = values.split(',');

  bool isHsla = text.startsWith("hsla");

  if ((!isHsla && parts.size() != 3) || (isHsla && parts.size() != 4)) { return std::nullopt; }

  try {
    int h = parts[0].trimmed().toInt();
    double s = parsePercentage(parts[1].trimmed());
    double l = parsePercentage(parts[2].trimmed());

    if (h < 0 || h >= 360 || s < 0 || s > 100 || l < 0 || l > 100) { return std::nullopt; }

    QColor color;
    color.setHsl(h, std::round(s * 2.55), std::round(l * 2.55));

    if (isHsla) {
      double alpha = parts[3].trimmed().toDouble();
      if (alpha < 0 || alpha > 1) return std::nullopt;
      color.setAlphaF(alpha);
      return ParsedColor{HSL_ALPHA, color};
    }

    return ParsedColor{HSL, color};
  } catch (...) { return std::nullopt; }
}

std::optional<ColorFormatter::ParsedColor> ColorFormatter::tryParseHsv(const QString &text) {
  QRegularExpression hsvRegex("^hsva?\\s*\\(\\s*([^)]+)\\s*\\)$");
  QRegularExpressionMatch match = hsvRegex.match(text);

  if (!match.hasMatch()) { return std::nullopt; }

  QString values = match.captured(1);
  QStringList parts = values.split(',');

  bool isHsva = text.startsWith("hsva");

  if ((!isHsva && parts.size() != 3) || (isHsva && parts.size() != 4)) { return std::nullopt; }

  try {
    int h = parts[0].trimmed().toInt();
    double s = parsePercentage(parts[1].trimmed());
    double v = parsePercentage(parts[2].trimmed());

    if (h < 0 || h >= 360 || s < 0 || s > 100 || v < 0 || v > 100) { return std::nullopt; }

    QColor color;
    color.setHsv(h, std::round(s * 2.55), std::round(v * 2.55));

    if (isHsva) {
      double alpha = parts[3].trimmed().toDouble();
      if (alpha < 0 || alpha > 1) return std::nullopt;
      color.setAlphaF(alpha);
      return ParsedColor{HSV_ALPHA, color};
    }

    return ParsedColor{HSV, color};
  } catch (...) { return std::nullopt; }
}

// Helper function to parse percentage values
double ColorFormatter::parsePercentage(const QString &text) {
  QString cleaned = text.trimmed();
  if (cleaned.endsWith('%')) {
    cleaned.chop(1);
    return cleaned.toDouble();
  }
  return cleaned.toDouble();
}
