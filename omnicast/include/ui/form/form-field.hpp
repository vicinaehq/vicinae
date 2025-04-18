#pragma once
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qwidget.h>

class FormField : public QWidget {
  TypographyWidget *_nameLabel;
  TypographyWidget *_errorLabel;
  QWidget *_widget;
  QHBoxLayout *_layout;

public:
  FormField(QWidget *widget = new QWidget, const QString &name = "");

  void setName(const QString &name);
  void setError(const QString &error);
  QString errorText() const;
  void setWidget(QWidget *widget);
  void clearError();

  QWidget *widget() const;
  void focus() const;
};
