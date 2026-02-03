#pragma once
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qwidget.h>

class TooltipWidget : public QWidget {
  QWidget *m_target = nullptr;
  QWidget *m_content = nullptr;
  Qt::Alignment m_alignment = Qt::AlignBottom | Qt::AlignCenter;
  QVBoxLayout *m_layout = new QVBoxLayout;

  void paintEvent(QPaintEvent *event) override;
  QPoint calculatePosition(Qt::Alignment align) const;
  void position();
  bool eventFilter(QObject *watched, QEvent *event) override;
  void showEvent(QShowEvent *event) override;
  void resizeEvent(QResizeEvent *event) override {
    position();
    QWidget::resizeEvent(event);
  }

public:
  TooltipWidget(QWidget *parent = nullptr);
  void setAlignment(Qt::Alignment align);
  void setWidget(QWidget *widget);
  void setTarget(QWidget *target);
  void setText(const QString &s);
};
