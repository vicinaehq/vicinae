#pragma once
#include "ui/focus-notifier.hpp"
#include "ui/typography/typography.hpp"
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
  bool m_vertical = false;
  TypographyWidget *m_info = new TypographyWidget;
  QWidget *_widget;
  QHBoxLayout *_layout;
  QWidget *m_infoContainer = new QWidget;
  QVBoxLayout *m_mainLayout = new QVBoxLayout(this);
  QHBoxLayout *m_infoLayout = new QHBoxLayout();

protected:
  void focusInEvent(QFocusEvent *event) override { QWidget::focusInEvent(event); }

  void focusOutEvent(QFocusEvent *event) override { QWidget::focusInEvent(event); }

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::FocusIn) {
      emit focusChanged(true);
    } else if (event->type() == QEvent::FocusOut) {
      emit focusChanged(false);
    }

    return false;
  }

public:
  FormField(QWidget *widget = new QWidget, const QString &name = "");

  void setVerticalDirection(bool value);
  void setName(const QString &name);
  void setError(const QString &error);
  void setInfo(const QString &info);
  QString errorText() const;
  void setWidget(QWidget *widget, FocusNotifier *focusNotifier = nullptr);
  void clearError();
  bool hasError() const;

  QWidget *widget() const;
  void focus() const;

signals:
  void focusChanged(bool value) const;
  void blurred() const;
  void focused() const;
};
