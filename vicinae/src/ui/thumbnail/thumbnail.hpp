#pragma once
#include "../image/url.hpp"
#include "ui/dialog/dialog.hpp"
#include "ui/image/image.hpp"
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

  ImageWidget *m_content = new ImageWidget(this);
  ImageWidget *m_placeholder = new ImageWidget(this);
  int m_borderRadius = 20;
  bool m_clickable = false;
  QGraphicsOpacityEffect *m_opacityEffect = new QGraphicsOpacityEffect(this);

  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void setupUI();
  void mousePressEvent(QMouseEvent *event) override;

public:
  void setClickable(bool clickable);
  void setImage(const ImageURL &url);
  void setRadius(int radius);

  Thumbnail(QWidget *parent = nullptr);

signals:
  void clicked() const;
};

class ImagePreviewDialogWidget : public DialogContentWidget {
  Thumbnail *m_thumbnail = new Thumbnail;
  int m_padding = 20;
  double m_aspectRatio = 16 / 9.f;

  void resizeEvent(QResizeEvent *event) override;
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;
  void setupUI();

public:
  void setAspectRatio(double ratio);

  ImagePreviewDialogWidget(const ImageURL &icon);
};
