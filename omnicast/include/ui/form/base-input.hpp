#pragma once
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include <qevent.h>
#include <qjsonobject.h>
#include <qlineedit.h>
#include <QFocusEvent>

class BaseInput : public QLineEdit, public IJsonFormField {
  FocusNotifier *m_focusNotifier = new FocusNotifier(this);
  QWidget *rightAccessory;
  QWidget *leftAccessory;

  void paintEvent(QPaintEvent *event) override;
  bool event(QEvent *) override;
  void resizeEvent(QResizeEvent *event) override;
  void setFocusState(bool value);
  void recalculate();

  QJsonValue asJsonValue() const override;
  void setValueAsJson(const QJsonValue &value) override;

public:
  void setLeftAccessory(QWidget *widget);
  void setRightAccessory(QWidget *widget);
  FocusNotifier *focusNotifier() const { return m_focusNotifier; }

  BaseInput(QWidget *parent = nullptr);
};
