#pragma once
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include <qcoreevent.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qwidget.h>
#include "ui/form/json-form-item-widget.hpp"

class Switch : public JsonFormItemWidget {
  bool m_value = false;
  FocusNotifier *m_focusNotifier = new FocusNotifier(this);

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
  bool event(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;

public:
  FocusNotifier *focusNotifier() const override;
  QJsonValue asJsonValue() const override;
  void setValueAsJson(const QJsonValue &value) override;

  bool value() const;
  void setValue(bool value);
  void toggle();

  Switch();
};
