#pragma once
#include <qgraphicseffect.h>
#include <qnamespace.h>
#include <qpropertyanimation.h>
#include <qwidget.h>
#include "ui/image/image.hpp"

/*
class FadeWidget : public QWidget {
private:
  QGraphicsOpacityEffect *opacityEffect;
  QPropertyAnimation *fadeAnimation;

public:
  FadeWidget(QWidget *parent = nullptr) : QWidget(parent) {
    // Create opacity effect
    opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opacityEffect);

    // Create animation
    fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeAnimation->setDuration(200); // 300ms duration
  }

  void fadeIn() {
    show(); // Make sure widget is visible
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start();
  }

  void showDirect() {
    opacityEffect->setOpacity(1.0);
    show();
  }

  void fadeOut() {
    if (!isVisible()) return;

    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);

    // Hide widget when animation finishes
    connect(fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
      hide();
      disconnect(fadeAnimation, &QPropertyAnimation::finished, this, nullptr);
    });

    fadeAnimation->start();
  }
};
*/

class TypographyWidget;

class HudWidget : public QWidget {
  TypographyWidget *m_text = nullptr;
  ImageWidget *m_icon = nullptr;
  bool m_shouldDrawBorders = true;

  void paintEvent(QPaintEvent *event) override;
  void setupUI();

public:
  void setText(const QString &text);
  void setIcon(const ImageURL &icon);
  void setClientSideBorderDrawing(bool value);
  void clear();

  HudWidget() { setupUI(); }
};
