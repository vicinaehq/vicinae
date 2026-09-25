#pragma once

#include <QByteArray>
#include <QColor>
#include <QRectF>
#include <QSizeF>
#include <QString>
#include <QUrl>
#include <memory>
#include <optional>
#include <vector>

class QPainter;

namespace math {
struct Resource {
  QString url;
  QSizeF size;
  QByteArray svg;
  ~Resource();
};

using Resources = std::vector<std::shared_ptr<const Resource>>;

// Documents retain the typeset resources. Short URLs carry only their identity,
// keeping image payloads out of the HTML that Qt uses to classify large text documents.
std::shared_ptr<const Resource> render(const QString &latex, bool display, const QColor &color);
std::shared_ptr<const Resource> resource(const QUrl &url);
std::optional<QSizeF> size(const QUrl &url, qreal fontSize);
void draw(QPainter *painter, const QRectF &rect, const QUrl &url);
} // namespace math
