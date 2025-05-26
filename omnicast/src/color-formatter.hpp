#pragma once

#include <QColor>
#include <QString>
#include <QRgb>
#include <expected>
#include <optional>

class ColorFormatter {
public:
  enum ColorFormat {
    Invalid,
    RGB,
    RGBA,
    RGBA_PERCENTAGE,
    HEX,
    HEX_ALPHA,
    HSL,
    HSL_ALPHA,
    HSV,
    HSV_ALPHA,
  };

  struct ParsedColor {
    ColorFormat format;
    QColor color;
  };

  // Enhanced error system to differentiate between different types of errors
  struct GenericError {
    enum Type {
      NotAColorFormat, // String doesn't match any color format pattern
      InvalidRange,    // Values are out of valid range
      InvalidSyntax,   // Syntax error in format
      ConversionError  // Error during color conversion
    };

    Type type;
    QString message;

    GenericError(Type t, const QString &msg = "") : type(t), message(msg) {}
  };

  /**
   * Format the color to the given format.
   */
  QString format(const QColor &color, ColorFormat format);

  QString formatName(ColorFormat format) const;

  std::vector<ColorFormat> formats() const;

  /**
   * Parse provided text to QColor, with format information.
   */
  std::expected<ParsedColor, GenericError> parse(const QString &text);

  ColorFormatter() = default;

private:
  // Formatting functions
  QString formatRgb(const QColor &color);
  QString formatRgba(const QColor &color);
  QString formatRgbaPercentage(const QColor &color);
  QString formatHex(const QColor &color);
  QString formatHexAlpha(const QColor &color);
  QString formatHsl(const QColor &color);
  QString formatHslAlpha(const QColor &color);
  QString formatHsv(const QColor &color);
  QString formatHsvAlpha(const QColor &color);

  // Parsing functions
  std::optional<ParsedColor> tryParseHex(const QString &text);
  std::optional<ParsedColor> tryParseRgb(const QString &text);
  std::optional<ParsedColor> tryParseHsl(const QString &text);
  std::optional<ParsedColor> tryParseHsv(const QString &text);
  std::optional<ParsedColor> tryParseNamed(const QString &text);

  // Helper functions
  double parsePercentage(const QString &text);
};
