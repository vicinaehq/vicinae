#pragma once
#include <QQuickPaintedItem>
#include <QPainter>
#include <QPainterPath>
#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGMaterial>

/// A rounded rectangle that composites with Source blend mode at the scene
/// graph level, replicating QPainter-based backgrounds that punch through
/// overlapping items to show correct alpha to the desktop.
///
/// Because blending is fully disabled on this item's texture, every pixel
/// in the bounding box replaces the destination.
///
/// Two usage modes:
///  1. **Inside a frame** (grid cells, list items): set `backgroundColor`
///     to the frame's fill colour so the corners match the surrounding frame.
///     SourceAtop is used internally to avoid alpha fringe at rounded edges.
///  2. **Window frame** itself: leave `backgroundColor` transparent.
///     Source mode is used directly, giving smooth antialiased edges to the
///     desktop.  Optionally set `secondaryColor`/`secondaryHeight` to paint
///     a distinct bottom zone (e.g. status bar).
class QmlSourceBlendRect : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
  Q_PROPERTY(QColor secondaryColor READ secondaryColor WRITE setSecondaryColor NOTIFY secondaryColorChanged)
  Q_PROPERTY(qreal secondaryHeight READ secondaryHeight WRITE setSecondaryHeight NOTIFY secondaryHeightChanged)

public:
  explicit QmlSourceBlendRect(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {
    setAntialiasing(true);
    setFillColor(Qt::transparent);
  }

  QColor color() const { return m_color; }
  void setColor(const QColor &c) {
    if (m_color != c) { m_color = c; emit colorChanged(); update(); }
  }

  qreal radius() const { return m_radius; }
  void setRadius(qreal r) {
    if (m_radius != r) { m_radius = r; emit radiusChanged(); update(); }
  }

  QColor borderColor() const { return m_borderColor; }
  void setBorderColor(const QColor &c) {
    if (m_borderColor != c) { m_borderColor = c; emit borderColorChanged(); update(); }
  }

  int borderWidth() const { return m_borderWidth; }
  void setBorderWidth(int w) {
    if (m_borderWidth != w) { m_borderWidth = w; emit borderWidthChanged(); update(); }
  }

  QColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const QColor &c) {
    if (m_backgroundColor != c) { m_backgroundColor = c; emit backgroundColorChanged(); update(); }
  }

  QColor secondaryColor() const { return m_secondaryColor; }
  void setSecondaryColor(const QColor &c) {
    if (m_secondaryColor != c) { m_secondaryColor = c; emit secondaryColorChanged(); update(); }
  }

  qreal secondaryHeight() const { return m_secondaryHeight; }
  void setSecondaryHeight(qreal h) {
    if (m_secondaryHeight != h) { m_secondaryHeight = h; emit secondaryHeightChanged(); update(); }
  }

  void paint(QPainter *painter) override {
    painter->setRenderHint(QPainter::Antialiasing, true);

    const bool hasBackground = m_backgroundColor.alpha() > 0;

    // 1. Fill entire bounds with the background colour.
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(boundingRect(), m_backgroundColor);

    // The single shape path — fill and border both use the full rounded rect,
    // just like the widget paints the full path then strokes on top.
    QPainterPath shapePath;
    shapePath.addRoundedRect(boundingRect(), m_radius, m_radius);

    // 2. Main fill — covers the entire rounded rect including border area.
    painter->setPen(Qt::NoPen);
    if (hasBackground) {
      painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
      painter->fillPath(shapePath, QColor(m_color.red(), m_color.green(),
                                          m_color.blue(), 255));
    } else {
      painter->setCompositionMode(QPainter::CompositionMode_Source);
      painter->fillPath(shapePath, m_color);
    }

    // 3. Optional secondary zone at the bottom (e.g. status bar).
    if (m_secondaryHeight > 0 && m_secondaryColor.alpha() > 0) {
      painter->setCompositionMode(QPainter::CompositionMode_Source);
      QRectF secRect(boundingRect().x(),
                     boundingRect().bottom() - m_secondaryHeight,
                     boundingRect().width(), m_secondaryHeight);
      painter->setClipPath(shapePath);
      painter->fillRect(secRect, m_secondaryColor);
      painter->setClipping(false);
    }

    // 4. Border outline — clipped to the rounded rect, drawn on top.
    if (m_borderWidth > 0 && m_borderColor.alpha() > 0) {
      painter->setClipPath(shapePath);
      if (hasBackground) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter->setPen(QPen(QColor(m_borderColor.red(), m_borderColor.green(),
                                    m_borderColor.blue(), 255), m_borderWidth));
      } else {
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->setPen(QPen(m_borderColor, m_borderWidth));
      }
      painter->setBrush(Qt::NoBrush);
      painter->drawPath(shapePath);
    }
  }

  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override {
    auto *node = QQuickPaintedItem::updatePaintNode(oldNode, data);
    if (node)
      disableBlending(node);
    return node;
  }

private:
  static void disableBlending(QSGNode *node) {
    if (node->type() == QSGNode::GeometryNodeType) {
      auto *geo = static_cast<QSGGeometryNode *>(node);
      if (auto *mat = geo->material())
        mat->setFlag(QSGMaterial::Blending, false);
      if (auto *mat = geo->opaqueMaterial())
        mat->setFlag(QSGMaterial::Blending, false);
    }
    for (auto *child = node->firstChild(); child; child = child->nextSibling())
      disableBlending(child);
  }

signals:
  void colorChanged();
  void radiusChanged();
  void borderColorChanged();
  void borderWidthChanged();
  void backgroundColorChanged();
  void secondaryColorChanged();
  void secondaryHeightChanged();

private:
  QColor m_color = Qt::transparent;
  qreal m_radius = 0;
  QColor m_borderColor = Qt::transparent;
  int m_borderWidth = 0;
  QColor m_backgroundColor = Qt::transparent;
  QColor m_secondaryColor = Qt::transparent;
  qreal m_secondaryHeight = 0;
};
