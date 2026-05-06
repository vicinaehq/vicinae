#pragma once
#include <QQuickItem>
#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QtQml/qqmlregistration.h>

class SourceBlendMaterial : public QSGMaterial {
public:
  QSGMaterialType *type() const override;
  QSGMaterialShader *createShader(QSGRendererInterface::RenderMode) const override;
  int compare(const QSGMaterial *other) const override;

  QColor color;
  QColor backgroundColor;
  QColor secondaryColor;
  QColor borderColor;
  QSizeF itemSize;
  float radius = 0;
  float borderWidth = 0;
  float secondaryHeight = 0;
  int mode = 0; // 0=normal, 1=cornerMask, 2=overlay
  bool sourceBlend = false;
};

class SourceBlendRect : public QQuickItem {
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
  Q_PROPERTY(bool cornerMask READ cornerMask WRITE setCornerMask NOTIFY cornerMaskChanged)

signals:
  void colorChanged();
  void radiusChanged();
  void borderColorChanged();
  void borderWidthChanged();
  void backgroundColorChanged();
  void secondaryColorChanged();
  void secondaryHeightChanged();
  void overlayChanged();
  void cornerMaskChanged();

public:
  explicit SourceBlendRect(QQuickItem *parent = nullptr);

  QColor color() const { return m_color; }
  void setColor(const QColor &c);
  qreal radius() const { return m_radius; }
  void setRadius(qreal r);
  QColor borderColor() const { return m_borderColor; }
  void setBorderColor(const QColor &c);
  int borderWidth() const { return m_borderWidth; }
  void setBorderWidth(int w);
  QColor backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const QColor &c);
  QColor secondaryColor() const { return m_secondaryColor; }
  void setSecondaryColor(const QColor &c);
  qreal secondaryHeight() const { return m_secondaryHeight; }
  void setSecondaryHeight(qreal h);
  bool overlay() const { return m_overlay; }
  void setOverlay(bool v);
  bool cornerMask() const { return m_cornerMask; }
  void setCornerMask(bool v);

protected:
  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
  QColor m_color = Qt::transparent;
  qreal m_radius = 0;
  QColor m_borderColor = Qt::transparent;
  int m_borderWidth = 0;
  QColor m_backgroundColor = Qt::transparent;
  QColor m_secondaryColor = Qt::transparent;
  qreal m_secondaryHeight = 0;
  bool m_overlay = false;
  bool m_cornerMask = false;
};
