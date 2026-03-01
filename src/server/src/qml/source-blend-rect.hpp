#pragma once
#include <QQuickPaintedItem>
#include <QQuickWindow>
#include <QPainter>
#include <QPainterPath>
#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGRendererInterface>
#include <QtQml/qqmlregistration.h>

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
class SourceBlendRect : public QQuickPaintedItem {
  Q_OBJECT
  QML_NAMED_ELEMENT(SourceBlendRect)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
  Q_PROPERTY(
      QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
  Q_PROPERTY(QColor secondaryColor READ secondaryColor WRITE setSecondaryColor NOTIFY secondaryColorChanged)
  Q_PROPERTY(
      qreal secondaryHeight READ secondaryHeight WRITE setSecondaryHeight NOTIFY secondaryHeightChanged)
  Q_PROPERTY(bool overlay READ overlay WRITE setOverlay NOTIFY overlayChanged)

public:
  explicit SourceBlendRect(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {
    setAntialiasing(true);
    setFillColor(Qt::transparent);
  }

  QColor color() const { return m_color; }
  void setColor(const QColor &c) {
    if (m_color != c) {
      m_color = c;
      emit colorChanged();
      update();
    }
  }

  qreal radius() const { return m_radius; }
  void setRadius(qreal r) {
    if (m_radius != r) {
      m_radius = r;
      emit radiusChanged();
      update();
    }
  }

  QColor borderColor() const { return m_borderColor; }
  void setBorderColor(const QColor &c) {
    if (m_borderColor != c) {
      m_borderColor = c;
      emit borderColorChanged();
      update();
    }
  }

  int borderWidth() const { return m_borderWidth; }
  void setBorderWidth(int w) {
    if (m_borderWidth != w) {
      m_borderWidth = w;
      emit borderWidthChanged();
      update();
    }
  }

  QColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const QColor &c) {
    if (m_backgroundColor != c) {
      m_backgroundColor = c;
      emit backgroundColorChanged();
      update();
    }
  }

  QColor secondaryColor() const { return m_secondaryColor; }
  void setSecondaryColor(const QColor &c) {
    if (m_secondaryColor != c) {
      m_secondaryColor = c;
      emit secondaryColorChanged();
      update();
    }
  }

  qreal secondaryHeight() const { return m_secondaryHeight; }
  void setSecondaryHeight(qreal h) {
    if (m_secondaryHeight != h) {
      m_secondaryHeight = h;
      emit secondaryHeightChanged();
      update();
    }
  }

  bool overlay() const { return m_overlay; }
  void setOverlay(bool v) {
    if (m_overlay != v) {
      m_overlay = v;
      emit overlayChanged();
      update();
    }
  }

  void paint(QPainter *painter) override {
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (m_overlay) {
      paintOverlay(painter);
      return;
    }

    const bool hasBackground = m_backgroundColor.alpha() > 0;

    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(boundingRect(), m_backgroundColor);

    QPainterPath shapePath;
    shapePath.addRoundedRect(boundingRect(), m_radius, m_radius);

    painter->setPen(Qt::NoPen);
    if (hasBackground) {
      painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
      painter->fillPath(shapePath, QColor(m_color.red(), m_color.green(), m_color.blue(), 255));
    } else {
      painter->setCompositionMode(QPainter::CompositionMode_Source);
      painter->fillPath(shapePath, m_color);
    }

    if (m_secondaryHeight > 0 && m_secondaryColor.alpha() > 0) {
      painter->setCompositionMode(QPainter::CompositionMode_Source);
      QRectF secRect(boundingRect().x(), boundingRect().bottom() - m_secondaryHeight, boundingRect().width(),
                     m_secondaryHeight);
      painter->setClipPath(shapePath);
      painter->fillRect(secRect, m_secondaryColor);
      painter->setClipping(false);
    }

    if (m_borderWidth > 0 && m_borderColor.alpha() > 0) {
      painter->setClipPath(shapePath);
      if (hasBackground) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter->setPen(QPen(QColor(m_borderColor.red(), m_borderColor.green(), m_borderColor.blue(), 255),
                             m_borderWidth));
      } else {
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->setPen(QPen(m_borderColor, m_borderWidth));
      }
      painter->setBrush(Qt::NoBrush);
      painter->drawPath(shapePath);
    }
  }

  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override {
    if (width() <= 0 || height() <= 0) {
      delete oldNode;
      return nullptr;
    }
    auto *node = QQuickPaintedItem::updatePaintNode(oldNode, data);
    if (node && !m_overlay && window()->rendererInterface()->graphicsApi() != QSGRendererInterface::Software)
      disableBlending(node);
    return node;
  }

private:
  /// Overlay mode: paints only corner mask + border with normal alpha blending.
  /// Content behind the interior shows through; corners are opaque-filled to
  /// give a rounded-clip appearance.
  ///
  /// The item should be sized with `overlayPadding` px extra on each side
  /// so the border ring (1px outside the cell) fits in the texture.
  static constexpr qreal overlayPadding = 2;

  void paintOverlay(QPainter *painter) {
    if (m_borderWidth <= 0 || m_borderColor.alpha() <= 0) return;

    QRectF cellRect =
        boundingRect().adjusted(overlayPadding, overlayPadding, -overlayPadding, -overlayPadding);

    painter->setRenderHint(QPainter::Antialiasing, true);
    QPainterPath borderPath;
    borderPath.addRoundedRect(cellRect, m_radius, m_radius);
    painter->setPen(QPen(m_borderColor, m_borderWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(borderPath);
  }

  static void disableBlending(QSGNode *node) {
    if (node->type() == QSGNode::GeometryNodeType) {
      auto *geo = static_cast<QSGGeometryNode *>(node);
      if (auto *mat = geo->material()) mat->setFlag(QSGMaterial::Blending, false);
      if (auto *mat = geo->opaqueMaterial()) mat->setFlag(QSGMaterial::Blending, false);
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
  void overlayChanged();

private:
  QColor m_color = Qt::transparent;
  qreal m_radius = 0;
  QColor m_borderColor = Qt::transparent;
  int m_borderWidth = 0;
  QColor m_backgroundColor = Qt::transparent;
  QColor m_secondaryColor = Qt::transparent;
  qreal m_secondaryHeight = 0;
  bool m_overlay = false;
};
