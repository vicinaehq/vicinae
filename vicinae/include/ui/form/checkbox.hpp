#pragma once
#include "common.hpp"
#include "theme.hpp"
#include "ui/focus-notifier.hpp"
#include <qcoreevent.h>
#include <qnamespace.h>
#include <qsvgrenderer.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class Checkbox : public JsonFormItemWidget {
  Q_OBJECT

  bool m_value = false;
  QSvgRenderer *m_svg = new QSvgRenderer(this);
  ColorLike m_fillColor = Qt::transparent;
  FocusNotifier *m_focusNotifier = new FocusNotifier(this);

protected:
  void paintEvent(QPaintEvent *event) override;
  QSize sizeHint() const override;
  void keyPressEvent(QKeyEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  bool event(QEvent *event) override;

public:
  FocusNotifier *focusNotifier() const override;
  QJsonValue asJsonValue() const override;
  void setValueAsJson(const QJsonValue &value) override;
  bool value() const;
  void toggle();
  void setValue(bool value);
  void setFillColor(const ColorLike &color);

  // setValue but does not notify
  void stealthySetValue(bool value);

  Checkbox(QWidget *parent = nullptr);

signals:
  bool valueChanged(bool value) const;
};
