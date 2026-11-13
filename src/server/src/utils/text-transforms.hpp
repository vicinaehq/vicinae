#pragma once
#include <QColor>
#include <QString>
#include <optional>

namespace text_transforms {

QString toUppercase(const QString &text);
QString toLowercase(const QString &text);
QString toTitleCase(const QString &text);
QString toSlug(const QString &text);
QString trim(const QString &text);

/**
 * Check if the text matches a color representation (#hex, rgb(...), hsl(...))
 * and returns the parsed QColor if valid.
 */
std::optional<QColor> parseColor(const QString &text);

QString formatHex(const QColor &color);
QString formatRgb(const QColor &color);
QString formatHsl(const QColor &color);

} // namespace text_transforms
