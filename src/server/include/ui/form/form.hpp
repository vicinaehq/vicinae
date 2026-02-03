#pragma once
#include "ui/form/form-field.hpp"
#include <qwidget.h>

class FormWidget : public QWidget {
  std::vector<FormField *> m_fields;
  QVBoxLayout *_layout;

public:
  FormField *fieldForWidget(QWidget *widget) const;
  QWidget *widgetForField(FormField *field) const;

  void setError(QWidget *widget, const QString &error);
  void clearError(QWidget *widget);
  void clearAllErrors();
  bool isValid() const;
  void focusFirst() const;
  const std::vector<FormField *> fields() const;

  std::optional<FormField *> fieldAt(int index) const;
  void addField(FormField *field);
  FormField *addField();
  FormField *addField(const QString &label, JsonFormItemWidget *widget);

  // remove all fields
  void clearFields();

  void addSeparator();
  bool validate();

  FormWidget(QWidget *parent = nullptr);
};
