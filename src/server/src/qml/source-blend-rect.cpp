#include "source-blend-rect.hpp"
#include <QSGGeometry>
#include <QSGRendererInterface>
#include <cstring>

static void colorToFloat4(const QColor &c, float out[4]) {
  out[0] = static_cast<float>(c.redF());
  out[1] = static_cast<float>(c.greenF());
  out[2] = static_cast<float>(c.blueF());
  out[3] = static_cast<float>(c.alphaF());
}

class SourceBlendShader : public QSGMaterialShader {
public:
  SourceBlendShader() {
    setShaderFileName(VertexStage, QStringLiteral(":/shaders/src/qml/shaders/sourceblend.vert.qsb"));
    setShaderFileName(FragmentStage, QStringLiteral(":/shaders/src/qml/shaders/sourceblend.frag.qsb"));
    setFlag(UpdatesGraphicsPipelineState, true);
  }

  bool updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *) override {
    auto *mat = static_cast<SourceBlendMaterial *>(newMaterial);
    QByteArray *buf = state.uniformData();

    const QMatrix4x4 m = state.combinedMatrix();
    memcpy(buf->data(), m.constData(), 64);

    float opacity = state.opacity();
    memcpy(buf->data() + 64, &opacity, 4);

    float color[4];
    colorToFloat4(mat->color, color);
    memcpy(buf->data() + 80, color, 16);

    float bg[4];
    colorToFloat4(mat->backgroundColor, bg);
    memcpy(buf->data() + 96, bg, 16);

    float sec[4];
    colorToFloat4(mat->secondaryColor, sec);
    memcpy(buf->data() + 112, sec, 16);

    float border[4];
    colorToFloat4(mat->borderColor, border);
    memcpy(buf->data() + 128, border, 16);

    float itemSize[2] = {static_cast<float>(mat->itemSize.width()),
                         static_cast<float>(mat->itemSize.height())};
    memcpy(buf->data() + 144, itemSize, 8);

    float radius = mat->radius;
    memcpy(buf->data() + 152, &radius, 4);

    float borderWidth = static_cast<float>(mat->borderWidth);
    memcpy(buf->data() + 156, &borderWidth, 4);

    float secondaryHeight = mat->secondaryHeight;
    memcpy(buf->data() + 160, &secondaryHeight, 4);

    int mode = mat->mode;
    memcpy(buf->data() + 164, &mode, 4);

    return true;
  }

  bool updateGraphicsPipelineState(RenderState &, GraphicsPipelineState *ps, QSGMaterial *newMaterial,
                                   QSGMaterial *) override {
    auto *mat = static_cast<SourceBlendMaterial *>(newMaterial);
    if (mat->sourceBlend) {
      ps->blendEnable = true;
      ps->srcColor = GraphicsPipelineState::One;
      ps->dstColor = GraphicsPipelineState::Zero;
      ps->srcAlpha = GraphicsPipelineState::One;
      ps->dstAlpha = GraphicsPipelineState::Zero;
      return true;
    }
    return false;
  }
};

QSGMaterialType *SourceBlendMaterial::type() const {
  static QSGMaterialType t;
  return &t;
}

QSGMaterialShader *SourceBlendMaterial::createShader(QSGRendererInterface::RenderMode) const {
  return new SourceBlendShader;
}

int SourceBlendMaterial::compare(const QSGMaterial *other) const {
  auto *o = static_cast<const SourceBlendMaterial *>(other);
  if (sourceBlend != o->sourceBlend) return sourceBlend ? -1 : 1;
  if (mode != o->mode) return mode - o->mode;
  if (color != o->color) return color.rgba() < o->color.rgba() ? -1 : 1;
  if (backgroundColor != o->backgroundColor)
    return backgroundColor.rgba() < o->backgroundColor.rgba() ? -1 : 1;
  if (radius != o->radius) return radius < o->radius ? -1 : 1;
  if (borderWidth != o->borderWidth) return borderWidth < o->borderWidth ? -1 : 1;
  if (itemSize != o->itemSize) return itemSize.width() < o->itemSize.width() ? -1 : 1;
  return 0;
}

SourceBlendRect::SourceBlendRect(QQuickItem *parent) : QQuickItem(parent) {
  setFlag(ItemHasContents, true);
}

void SourceBlendRect::setColor(const QColor &c) {
  if (m_color != c) { m_color = c; emit colorChanged(); update(); }
}

void SourceBlendRect::setRadius(qreal r) {
  if (m_radius != r) { m_radius = r; emit radiusChanged(); update(); }
}

void SourceBlendRect::setBorderColor(const QColor &c) {
  if (m_borderColor != c) { m_borderColor = c; emit borderColorChanged(); update(); }
}

void SourceBlendRect::setBorderWidth(int w) {
  if (m_borderWidth != w) { m_borderWidth = w; emit borderWidthChanged(); update(); }
}

void SourceBlendRect::setBackgroundColor(const QColor &c) {
  if (m_backgroundColor != c) { m_backgroundColor = c; emit backgroundColorChanged(); update(); }
}

void SourceBlendRect::setSecondaryColor(const QColor &c) {
  if (m_secondaryColor != c) { m_secondaryColor = c; emit secondaryColorChanged(); update(); }
}

void SourceBlendRect::setSecondaryHeight(qreal h) {
  if (m_secondaryHeight != h) { m_secondaryHeight = h; emit secondaryHeightChanged(); update(); }
}

void SourceBlendRect::setOverlay(bool v) {
  if (m_overlay != v) { m_overlay = v; emit overlayChanged(); update(); }
}

void SourceBlendRect::setCornerMask(bool v) {
  if (m_cornerMask != v) { m_cornerMask = v; emit cornerMaskChanged(); update(); }
}

QSGNode *SourceBlendRect::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
  if (width() <= 0 || height() <= 0) {
    delete oldNode;
    return nullptr;
  }

  auto *node = static_cast<QSGGeometryNode *>(oldNode);
  SourceBlendMaterial *material = nullptr;

  if (!node) {
    node = new QSGGeometryNode;
    auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
    geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);

    material = new SourceBlendMaterial;
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);
  } else {
    material = static_cast<SourceBlendMaterial *>(node->material());
  }

  QSGGeometry::updateTexturedRectGeometry(node->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
  node->markDirty(QSGNode::DirtyGeometry);

  material->color = m_color;
  material->backgroundColor = m_backgroundColor;
  material->secondaryColor = m_secondaryColor;
  material->borderColor = m_borderColor;
  material->itemSize = QSizeF(width(), height());
  material->radius = static_cast<float>(m_radius);
  material->borderWidth = m_borderWidth;
  material->secondaryHeight = static_cast<float>(m_secondaryHeight);

  if (m_cornerMask)
    material->mode = 1;
  else if (m_overlay)
    material->mode = 2;
  else
    material->mode = 0;

  material->sourceBlend = (material->mode == 0 && m_backgroundColor.alphaF() > 0.0);
  material->setFlag(QSGMaterial::Blending, true);
  node->markDirty(QSGNode::DirtyMaterial);
  return node;
}
