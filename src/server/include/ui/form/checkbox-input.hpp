#pragma once
#include "common.hpp"
#include "ui/form/checkbox.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class CheckboxInput : public JsonFormItemWidget {
  Q_OBJECT

  TypographyWidget *m_label = new TypographyWidget;
  Checkbox *m_checkbox = new Checkbox;

  QJsonValue asJsonValue() const override { return m_checkbox->value(); }

public:
  void setLabel(const QString &label) {
    m_label->setText(label);
    m_label->setVisible(!label.isEmpty());
  }
  void setValueAsJson(const QJsonValue &value) override { m_checkbox->setValue(value.toBool(false)); }
  void stealthySetValueAsJson(const QJsonValue &value) { m_checkbox->stealthySetValue(value.toBool(false)); }
  bool value() const { return m_checkbox->value(); }
  FocusNotifier *focusNotifier() const override { return m_checkbox->focusNotifier(); }

  CheckboxInput(QWidget *parent = nullptr) : JsonFormItemWidget(parent) {
    setFocusProxy(m_checkbox);
    setFocusPolicy(Qt::StrongFocus);
    m_checkbox->setFixedSize(16, 16);
    m_label->hide();

    HStack().add(m_checkbox).add(m_label).spacing(10).imbue(this);

    connect(m_checkbox, &Checkbox::valueChanged, this, &CheckboxInput::valueChanged);
  }

signals:
  void valueChanged(bool value) const;
};
