#pragma once
#include "layout.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/typography/typography.hpp"
#include <qpainterpath.h>
#include <qwidget.h>

class AliasBadge : public QWidget {
public:
  AliasBadge() {
    m_typography->setSize(TextSize::TextSmaller);
    m_typography->setAlignment(Qt::AlignCenter);

    VStack().add(m_typography, 0, Qt::AlignCenter).my(2).mx(10).imbue(this);
  }

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);
    QPainterPath path;

    painter.setBrush(Qt::NoBrush);
    painter.setThemePen(SemanticColor::BackgroundBorder);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    path.addRoundedRect(rect(), 4, 4);
    painter.setClipPath(path);
    painter.drawPath(path);
    QWidget::paintEvent(event);
  }

  void setText(const QString text) { m_typography->setText(text); }

private:
  TypographyWidget *m_typography = new TypographyWidget;
};
