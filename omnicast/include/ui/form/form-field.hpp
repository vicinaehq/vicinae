#pragma once
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

class FormField : public QWidget {
  QLabel *_nameLabel;
  QLabel *_errorLabel;
  QWidget *_widget;
  QHBoxLayout *_layout;

public:
  FormField(QWidget *widget, const QString &name = "");

  void setName(const QString &name);
  void setError(const QString &error);
  void clearError();

  QWidget *widget() const;
  void focus() const;
};
