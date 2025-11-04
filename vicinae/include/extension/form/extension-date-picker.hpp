#pragma once
#include "extend/form-model.hpp"
#include "extension/form/extension-form-input.hpp"
#include <qdatetime.h>
#include <qdatetimeedit.h>

class ExtensionDatePicker : public ExtensionFormInput {
  Q_OBJECT

  QDateTimeEdit *m_input = new QDateTimeEdit(this);
  std::shared_ptr<FormModel::DatePickerField> m_model;

  static QDateTime parseIso(const QString &s) {
    auto dt = QDateTime::fromString(s, Qt::ISODateWithMs);
    if (!dt.isValid()) dt = QDateTime::fromString(s, Qt::ISODate);
    return dt;
  }

  void handleChanged(const QDateTime &dt) {
    if (m_model && m_model->onChange) {
      m_extensionNotifier->notify(*m_model->onChange, dt.toString(Qt::ISODateWithMs));
    }
  }

public:
  void clear() override {
    // Reset to current date/time; if the field stores value, higher-level reset will override
    m_input->setDateTime(QDateTime::currentDateTime());
  }

  QJsonValue asJsonValue() const override { return m_input->dateTime().toString(Qt::ISODateWithMs); }

  void setValueAsJson(const QJsonValue &value) override {
    if (value.isString()) {
      auto dt = parseIso(value.toString());
      if (dt.isValid()) m_input->setDateTime(dt);
    }
  }

  void render(const std::shared_ptr<FormModel::IField> &field) override {
    m_model = std::static_pointer_cast<FormModel::DatePickerField>(field);

    const auto type = m_model->type.value_or("dateTime");

    if (type == "date") {
      m_input->setDisplayFormat("yyyy-MM-dd");
      // normalize to midnight when rendering an existing value
      if (auto v = m_model->value; v && v->isString()) {
        auto dt = parseIso(v->toString());
        if (dt.isValid()) {
          m_input->setDate(dt.date());
          m_input->setTime(QTime(0, 0));
        }
      }
    } else {
      m_input->setDisplayFormat("yyyy-MM-dd HH:mm");
      if (auto v = m_model->value; v && v->isString()) {
        auto dt = parseIso(v->toString());
        if (dt.isValid()) m_input->setDateTime(dt);
      }
    }

    // Min/Max
    if (auto min = m_model->min) {
      auto dt = parseIso(*min);
      if (dt.isValid()) m_input->setMinimumDateTime(dt);
    }
    if (auto max = m_model->max) {
      auto dt = parseIso(*max);
      if (dt.isValid()) m_input->setMaximumDateTime(dt);
    }

    // Default value if present and not already set
    if (auto value = m_model->value; value && value->isString()) {
      auto dt = parseIso(value->toString());
      if (dt.isValid()) m_input->setDateTime(dt);
    }
  }

  ExtensionDatePicker() {
    m_input->setCalendarPopup(true);
    m_input->setButtonSymbols(QAbstractSpinBox::NoButtons);
    setWrapped(m_input);
    connect(m_input, &QDateTimeEdit::dateTimeChanged, this, &ExtensionDatePicker::handleChanged);
  }
};
