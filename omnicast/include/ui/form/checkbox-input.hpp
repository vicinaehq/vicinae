#pragma once
#include "common.hpp"
#include "ui/form/checkbox.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

class CheckboxInput : public QWidget, public IJsonFormField {
  QHBoxLayout *m_layout = new QHBoxLayout;
  TypographyWidget *m_label = new TypographyWidget;
  Checkbox *m_checkbox = new Checkbox;

  void setValueAsJson(const QJsonValue &value) override { m_checkbox->setValue(value.toBool(false)); }

  QJsonValue asJsonValue() const override { return m_checkbox->value(); }

  void setLabel(const QString &label) {
    m_label->setText(label);
    m_label->setVisible(!label.isEmpty());
  }

public:
  CheckboxInput(QWidget *parent = nullptr) {
    m_checkbox->setFixedSize(20, 20);
    m_label->hide();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_label, 0, Qt::AlignLeft);
    m_layout->addWidget(m_checkbox, 0, Qt::AlignLeft);
    setLayout(m_layout);
  }
};
