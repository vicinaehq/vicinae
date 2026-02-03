#pragma once
#include <qnamespace.h>
#include <qpainter.h>
#include <qwidget.h>

class EmojiViewer : public QWidget {
  QString _emoji;
  int _pointSize;
  double _scaleHeight;
  Qt::AlignmentFlag _align;

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  void setEmoji(const QString &emoji);
  void setPointSize(int size);
  void setHeightScale(double factor);
  void setAlignment(Qt::AlignmentFlag);

  EmojiViewer(const QString &emoji);
};
