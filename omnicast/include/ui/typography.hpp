#pragma once
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qevent.h>
#include <qfont.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtextformat.h>
#include <qwidget.h>

class TypographyWidget : public QLabel {
  TextSize m_size = TextSize::TextRegular;
  QFont::Weight m_weight = QFont::Weight::Normal;
  ThemeService &m_theme = ThemeService::instance();
  ColorLike m_color = ColorTint::TextPrimary;

protected:
  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);
    QPalette pal = palette();

    pal.setBrush(QPalette::WindowText, painter.colorBrush(m_color));

    setPalette(pal);
    QLabel::paintEvent(event);
  }

public:
  void setColor(const ColorLike &color) {
    m_color = color;
    update();
  }

  void setFont(const QFont &f) {
    QFont font(f);

    font.setPointSize(m_theme.pointSize(m_size));
    font.setWeight(m_weight);
    QLabel::setFont(font);
  }

  void setFontWeight(QFont::Weight weight) {
    QFont _font = font();

    _font.setPointSize(m_theme.pointSize(m_size));
    _font.setWeight(weight);
    setFont(_font);

    m_weight = weight;
    updateGeometry();
  }

  void setSize(TextSize size) {
    QFont _font = font();

    _font.setPointSizeF(m_theme.pointSize(size));
    _font.setWeight(m_weight);

    setFont(_font);

    m_size = size;
    updateGeometry();
  }

  bool hasHeightForWidth() const override { return false; }

  TypographyWidget(QWidget *parent = nullptr) : QLabel(parent) { setSize(TextSize::TextRegular); }

  TypographyWidget(TextSize size, ColorTint color = ColorTint::TextPrimary, QWidget *parent = nullptr)
      : QLabel(parent), m_size(size), m_color(color) {
    setSize(size);
  }
};
