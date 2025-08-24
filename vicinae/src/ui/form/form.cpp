#include "ui/form/form.hpp"
#include "common.hpp"
#include "ui/form/form-field.hpp"
#include <qwidget.h>

FormField *FormWidget::fieldForWidget(QWidget *widget) const {
  for (const auto &field : _fields) {
    if (field->widget() == widget) return field;
  }

  return nullptr;
}

QWidget *FormWidget::widgetForField(FormField *field) const {
  for (const auto &f : _fields) {
    if (f == field) { return f->widget(); }
  }

  return nullptr;
}

void FormWidget::addField(FormField *field) {
  _fields.push_back(field);
  _layout->addWidget(field);
}

FormField *FormWidget::addField() {
  auto field = new FormField;

  addField(field);

  return field;
}

FormField *FormWidget::addField(const QString &label, QWidget *widget) {
  auto field = addField();

  field->setName(label);
  field->setWidget(widget);

  return field;
}

void FormWidget::clearFields() {
  for (const auto &field : _fields) {
    field->deleteLater();
  }
  clearAllErrors();
  _fields.clear();
}

bool FormWidget::isValid() const {
  for (const auto field : _fields) {
    if (auto error = field->errorText(); !error.isEmpty()) { return false; }
  }

  return true;
}

void FormWidget::addSeparator() { _layout->addWidget(new HDivider); }

void FormWidget::setError(QWidget *widget, const QString &error) {
  if (auto field = fieldForWidget(widget)) {
    field->setError(error);
    return;
  }

  qDebug() << "Form::setError() called on widget that does not belong to a field";
}

void FormWidget::clearError(QWidget *widget) { setError(widget, ""); }

const std::vector<FormField *> FormWidget::fields() const { return _fields; }

void FormWidget::clearAllErrors() {
  for (const auto &field : _fields) {
    field->clearError();
  }
}

std::optional<FormField *> FormWidget::fieldAt(int index) const {
  if (_fields.size() < index) return std::nullopt;

  return _fields.at(index);
}

void FormWidget::focusFirst() const {
  if (_fields.empty()) {
    qWarning() << "FormWidget::focusFirst() called on a form with no fields";
    return;
  }

  return _fields[0]->focus();
}

FormWidget::FormWidget(QWidget *parent) : QWidget(parent), _layout(new QVBoxLayout) {
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(10);
  _layout->setAlignment(Qt::AlignTop);

  setLayout(_layout);
}
