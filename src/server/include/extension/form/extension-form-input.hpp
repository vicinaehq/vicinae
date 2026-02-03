#pragma once
#include <qboxlayout.h>
#include <qjsonarray.h>
#include <qwidget.h>
#include "ui/form/json-form-item-widget.hpp"
#include "extend/form-model.hpp"
#include "ui/focus-notifier.hpp"

class ExtensionEventNotifier : public QObject {
  Q_OBJECT

public:
  ExtensionEventNotifier(QObject *parent = nullptr) : QObject(parent) {}

  void notify(const QString &id, const QJsonValue &value) { emit eventNotified(id, {value}); }

signals:
  void eventNotified(const QString &id, const QJsonArray &value) const;
};

class ExtensionFormInput : public JsonFormItemWidget {
  QVBoxLayout *m_layout = new QVBoxLayout(this);
  QWidget *m_widget = nullptr;
  FocusNotifier *m_focusNotifier = nullptr;
  std::shared_ptr<FormModel::IField> m_field;
  virtual void render(const std::shared_ptr<FormModel::IField> &field) {}

public:
  ExtensionEventNotifier *m_extensionNotifier = new ExtensionEventNotifier(this);

  virtual QJsonValue asJsonValue() const override = 0;

  virtual void reset() {
    if (m_field->storeValue) return;
    if (auto value = m_field->defaultValue) { setValueAsJson(*value); }

    clear();
  }

  virtual void clear() {}

  virtual void setValueAsJson(const QJsonValue &value) override = 0;

  ExtensionFormInput(QWidget *parent = nullptr) : JsonFormItemWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
  }

  FocusNotifier *focusNotifier() const override { return m_focusNotifier; }

  void setWrapped(QWidget *widget, FocusNotifier *focusNotifier = nullptr) {
    if (auto item = m_layout->takeAt(0); item && item->widget()) { item->widget()->deleteLater(); }

    m_layout->addWidget(widget);
    setFocusProxy(widget);

    m_focusNotifier = focusNotifier;
    m_widget = widget;
  }

  void dispatchRender(const std::shared_ptr<FormModel::IField> &field) {
    m_field = field;
    render(m_field);
  }
};
