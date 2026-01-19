#pragma once
#include <QWidget>
#include <qjsonvalue.h>

class FocusNotifier;

struct JsonFormItemWidget : public QWidget {
  virtual QJsonValue asJsonValue() const = 0;
  virtual void setValueAsJson(const QJsonValue &value) = 0;
  virtual FocusNotifier *focusNotifier() const = 0;

  JsonFormItemWidget(QWidget *parent = nullptr) : QWidget(parent) {}
};
