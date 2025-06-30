#pragma once
#include "theme.hpp"
#include <qevent.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qtextformat.h>
#include <qwidget.h>

class TypographyWidget : public QWidget {
  TextSize m_size = TextSize::TextRegular;
  QFont::Weight m_weight = QFont::Weight::Normal;
  ThemeService &m_theme = ThemeService::instance();
  ColorLike m_color = ColorTint::TextPrimary;
  QString m_text;
  QLabel *m_label = new QLabel(this);
  Qt::TextElideMode m_elideMode = Qt::ElideRight;

protected:
  QLabel *measurementLabel() const;

  void updateText();
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

public:
  void setText(const QString &text);
  QString text() const;
  void setEllideMode(Qt::TextElideMode mode);
  void setColor(const ColorLike &color);
  void setFont(const QFont &f);
  void setFontWeight(QFont::Weight weight);
  void setSize(TextSize size);
  void clear();
  void setAlignment(Qt::Alignment);
  void setWordWrap(bool wrap);

  TypographyWidget(QWidget *parent = nullptr);
};
