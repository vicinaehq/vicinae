#pragma once
#include "qfont.h"
#include "qwidget.h"
#include "theme.hpp"

class QLabel;
class ThemeService;

/**
 * A custom widget we specifically use to render individual pieces of text or paragraphs.
 *
 * It provides convenient features in that regard such as auto text ellision.
 *
 * It's still using a QLabel under the hood because QLabel does a lot of complex stuff
 * and does support rich text formatting (useful to embed links).
 *
 * We use a shared `measurementLabel` in order to provide sizeHint calculation of the non-ellided text
 * (and decide on whether or not we should perform ellision). That's because, due to the various features
 * QLabel supports, computing it ourselves is far too complex.
 */

class TypographyWidget : public QWidget {
  TextSize m_size = TextSize::TextRegular;
  QFont::Weight m_weight = QFont::Weight::Normal;
  ColorLike m_color;
  QString m_text;
  QLabel *m_label = nullptr;
  Qt::TextElideMode m_elideMode = Qt::ElideRight;
  bool m_autoEllide = true;

  // meant for visual debugging
  bool m_debugBackground = false;

protected:
  QLabel *measurementLabel() const;

  void updateText();
  void resizeEvent(QResizeEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  QSize minimumSizeHint() const override;

public:
  /**
   * Automatically ellide the label text (adding ... at the end) if it's not given
   * enough size to render the entire text.
   * The default is true.
   * Note that setting wordWrap to true will disable ellision no matter what auto ellide
   * has been set to.
   */
  QSize sizeHint() const override;
  void setAutoEllide(bool autoEllide = true);
  void setText(const QString &text);
  void setDebugHighlight();
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
