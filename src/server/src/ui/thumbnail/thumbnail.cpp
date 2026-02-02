#include "thumbnail.hpp"

void Thumbnail::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  m_content->setFixedSize(size());
  m_content->move(0, 0);
  m_content->show();
  m_placeholder->stackUnder(m_content);
}

void Thumbnail::paintEvent(QPaintEvent *event) {
  OmniPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing);

  painter.setThemePen(SemanticColor::BackgroundBorder);
  painter.setThemeBrush(SemanticColor::ListItemHoverBackground);
  painter.drawRoundedRect(rect(), m_borderRadius, m_borderRadius);

  m_opacityEffect->setOpacity(underMouse() && m_clickable ? 0.8 : 1);

  QWidget::paintEvent(event);
}

void Thumbnail::setupUI() {
  auto layout = new QVBoxLayout;

  m_placeholder->setFixedSize(25, 25);
  m_placeholder->setUrl(ImageURL::builtin("image").setFill(SemanticColor::TextMuted));
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_placeholder, 0, Qt::AlignCenter);

  setLayout(layout);
}

void Thumbnail::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::MouseButton::LeftButton && m_clickable) {
    emit clicked();
    return;
  }

  QWidget::mousePressEvent(event);
}

void Thumbnail::setClickable(bool clickable) {
  m_clickable = clickable;
  update();
}

void Thumbnail::setImage(const ImageURL &url) {
  m_content->setUrl(ImageURL(url).setMask(OmniPainter::RoundedRectangleMask));
}

void Thumbnail::setRadius(int radius) {
  m_borderRadius = radius;
  update();
}

Thumbnail::Thumbnail(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_Hover);
  setupUI();
  setGraphicsEffect(m_opacityEffect);
  m_opacityEffect->setOpacity(1);
}

void ImagePreviewDialogWidget::resizeEvent(QResizeEvent *event) {
  qDebug() << "resize preview dialog" << event;
  QWidget::resizeEvent(event);
}

QSize ImagePreviewDialogWidget::sizeHint() const {
  if (auto widget = parentWidget()) {
    auto parentSize = widget->size();
    int targetHeight = parentSize.height() - m_padding * 2;
    int width = targetHeight * m_aspectRatio;

    return {width, targetHeight};
  }

  return {100, 100};
}

QSize ImagePreviewDialogWidget::minimumSizeHint() const { return sizeHint(); }

void ImagePreviewDialogWidget::setupUI() {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_thumbnail);
  setLayout(layout);
}

void ImagePreviewDialogWidget::setAspectRatio(double ratio) { m_aspectRatio = ratio; }

ImagePreviewDialogWidget::ImagePreviewDialogWidget(const ImageURL &icon) {
  setupUI();
  m_thumbnail->setImage(icon);
}
