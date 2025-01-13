#pragma once

#include <qboxlayout.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class FormInputWidget : public QWidget {
  Q_OBJECT

  QLineEdit *input;
  virtual bool validate() { return true; };

public:
  FormInputWidget(const QString &id) : input(new QLineEdit) {
    connect(input, &QLineEdit::textChanged, this, &FormInputWidget::textChanged);

    auto layout = new QVBoxLayout();

    layout->addWidget(input);

    setLayout(layout);
  }

  QString text() { return input->text(); }
  void setText(const QString &value) {}
  void setError(const QString &error) {}

signals:
  void textChanged(const QString &text);
};

class FormWidget : public QWidget {
  QVBoxLayout *layout;

public:
  FormWidget() : layout(new QVBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    setLayout(layout);
  }

  void addInput(FormInputWidget *input) { layout->addWidget(input); }
};
