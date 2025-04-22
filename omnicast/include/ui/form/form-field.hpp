#pragma once
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qlabel.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class FormField : public QWidget {
  Q_OBJECT

  TypographyWidget *_nameLabel;
  TypographyWidget *_errorLabel;
  QWidget *_widget;
  QHBoxLayout *_layout;

protected:
  void focusInEvent(QFocusEvent *event) override {
    qDebug() << "focus in";
    QWidget::focusInEvent(event);
  }

  void focusOutEvent(QFocusEvent *event) override {
    qDebug() << "focus out";
    QWidget::focusInEvent(event);
  }

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (obj != _widget) return false;

    if (event->type() == QEvent::FocusIn) {
      emit focusChanged(true);
    } else if (event->type() == QEvent::FocusOut) {
      emit focusChanged(false);
    }

    return false;
  }

public:
  FormField(QWidget *widget = new QWidget, const QString &name = "");

  void setName(const QString &name);
  void setError(const QString &error);
  QString errorText() const;
  void setWidget(QWidget *widget);
  void clearError();
  bool hasError() const;

  QWidget *widget() const;
  void focus() const;

signals:
  void focusChanged(bool value);
};
