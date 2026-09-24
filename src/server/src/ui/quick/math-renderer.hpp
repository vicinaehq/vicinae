#pragma once

#include <QColor>
#include <QRectF>
#include <QSizeF>
#include <QString>
#include <QUrl>
#include <optional>

class QPainter;

namespace math {
// Typesetting runs on the Markdown worker. The resulting resource carries its metrics,
// so measuring an offscreen document never needs to decode or typeset an equation.
std::optional<QString> render(const QString &latex, bool display, const QColor &color);
std::optional<QSizeF> size(const QUrl &url, qreal fontSize);
void draw(QPainter *painter, const QRectF &rect, const QUrl &url);
} // namespace math
