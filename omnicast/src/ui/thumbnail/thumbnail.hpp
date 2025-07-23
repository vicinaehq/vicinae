#pragma once
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/dialog.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/omni-painter.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qgraphicseffect.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

/**
 * Show a thumbnail with a temporary placeholder when loading
 */

class Thumbnail : public QWidget {
  Q_OBJECT

  Omnimg::ImageWidget *m_content = new Omnimg::ImageWidget(this);
  Omnimg::ImageWidget *m_placeholder = new Omnimg::ImageWidget(this);
  int m_borderRadius = 20;
  bool m_clickable = false;
  QGraphicsOpacityEffect *m_opacityEffect = new QGraphicsOpacityEffect(this);

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    m_content->setFixedSize(size());
    m_content->move(0, 0);
    m_content->show();
    m_placeholder->stackUnder(m_content);
  }

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setThemePen(SemanticColor::Border);
    painter.setThemeBrush(SemanticColor::MainHoverBackground);
    painter.drawRoundedRect(rect(), m_borderRadius, m_borderRadius);

    m_opacityEffect->setOpacity(underMouse() && m_clickable ? 0.8 : 1);

    QWidget::paintEvent(event);
  }

  void setupUI() {
    auto layout = new QVBoxLayout;

    m_placeholder->setFixedSize(25, 25);
    m_placeholder->setUrl(BuiltinOmniIconUrl("image").setFill(SemanticColor::TextSecondary));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_placeholder, 0, Qt::AlignCenter);

    setLayout(layout);
  }

  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::MouseButton::LeftButton && m_clickable) {
      emit clicked();
      return;
    }

    QWidget::mousePressEvent(event);
  }

public:
  void setClickable(bool clickable) {
    m_clickable = clickable;
    update();
  }
  void setImage(const OmniIconUrl &url) {
    m_content->setUrl(OmniIconUrl(url).setMask(OmniPainter::RoundedRectangleMask));
  }
  void setRadius(int radius) {
    m_borderRadius = radius;
    update();
  }

  Thumbnail(QWidget *parent = nullptr) : QWidget(parent) {
    setAttribute(Qt::WA_Hover);
    setupUI();
    setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(1);
  }

signals:
  void clicked() const;
};

class ImagePreviewDialogWidget : public DialogContentWidget {
  Thumbnail *m_thumbnail = new Thumbnail;
  int m_padding = 20;
  double m_aspectRatio = 16 / 9.f;

  void resizeEvent(QResizeEvent *event) override {
    qDebug() << "resize preview dialog" << event;
    QWidget::resizeEvent(event);
  }

  QSize sizeHint() const override {
    if (auto widget = parentWidget()) {
      auto parentSize = widget->size();
      int targetHeight = parentSize.height() - m_padding * 2;
      int width = targetHeight * m_aspectRatio;

      qDebug() << "parent size" << parentSize;

      return {width, targetHeight};
    }

    return {100, 100};
  }

  QSize minimumSizeHint() const override { return sizeHint(); }

  void setupUI() {
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_thumbnail);
    setLayout(layout);
  }

public:
  void setAspectRatio(double ratio) { m_aspectRatio = ratio; }

  ImagePreviewDialogWidget(const OmniIconUrl &icon) {
    setupUI();
    m_thumbnail->setImage(icon);
  }
};
