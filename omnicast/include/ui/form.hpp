#pragma once

#include <qboxlayout.h>
#include <QPainterPath>
#include <qevent.h>
#include <qjsonvalue.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qsharedpointer.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>
#include <qwindowdefs.h>
#include "ui/form/form-field.hpp"

class FormWidget : public QWidget {
public:
  std::vector<FormField *> _fields;
  QVBoxLayout *_layout;

  FormField *fieldForWidget(QWidget *widget) const {
    for (const auto &field : _fields) {
      if (field->widget() == widget) return field;
    }

    return nullptr;
  }

  QWidget *widgetForField(FormField *field) const {
    for (const auto &f : _fields) {
      if (f == field) { return f->widget(); }
    }

    return nullptr;
  }

  void setError(QWidget *widget, const QString &error) {
    if (auto field = fieldForWidget(widget)) {
      field->setError(error);
      return;
    }

    qDebug() << "Form::setError() called on widget that does not belong to a field";
  }

  void clearError(QWidget *widget) { setError(widget, ""); }

  void clearAllErrors() {
    for (const auto &field : _fields) {
      field->clearError();
    }
  }

  void focusFirst() {
    if (_fields.empty()) {
      qWarning() << "FormWidget::focusFirst() called on a form with no fields";
      return;
    }

    return _fields[0]->focus();
  }

  const std::vector<FormField *> fields() const { return _fields; }

  FormWidget() : _layout(new QVBoxLayout) {
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(10);
    _layout->setAlignment(Qt::AlignTop);

    setLayout(_layout);
  }

  void addField(FormField *field) {
    _fields.push_back(field);
    _layout->addWidget(field);
  }
};
