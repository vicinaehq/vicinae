#pragma once
#include "common.hpp"
#include <qevent.h>
#include <qlineedit.h>
#include <QFocusEvent>

class BaseInput : public QLineEdit, public IJsonSerializable {
  QWidget *rightAccessory;
  QWidget *leftAccessory;
  bool _focused;

  void paintEvent(QPaintEvent *event) override;
  bool event(QEvent *) override;
  void resizeEvent(QResizeEvent *event) override;
  void setFocusState(bool value);
  void recalculate();

  QJsonValue asJsonValue() const override;

public:
  void setLeftAccessory(QWidget *widget);
  void setRightAccessory(QWidget *widget);

  BaseInput(QWidget *parent = nullptr);
};
