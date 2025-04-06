#pragma once
#include "theme.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qwindowdefs.h>

class DialogWidget : public QWidget {
  QVBoxLayout *_layout;
  QWidget *_content;

  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    QPainter painter(this);
    QColor finalColor(theme.colors.mainBackground);

    finalColor.setAlphaF(0.50);
    painter.setPen(Qt::NoPen);
    painter.fillRect(rect(), finalColor);

    QWidget::paintEvent(event);

    qDebug() << "dialog painting" << rect();
  }

  bool event(QEvent *event) override {
    if (event->type() == QEvent::MouseButtonPress) {
      close();
      return true;
    }

    return QWidget::event(event);
  }

public:
  void setContent(QWidget *content) {
    if (auto item = _layout->takeAt(0)) { item->widget()->deleteLater(); }

    _layout->addWidget(content, 0, Qt::AlignCenter);
  }

  void showDialog() {
    if (auto w = parentWidget()) {
      setGeometry(w->geometry());
      show();
    }
  }

  DialogWidget(QWidget *parent = nullptr) : QWidget(parent), _layout(new QVBoxLayout), _content(nullptr) {
    qDebug() << "dialog parent" << parent;
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);

    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    setLayout(_layout);
  }
};
