#pragma once
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qjsonobject.h>
#include <qlineedit.h>
#include <QFocusEvent>
#include <qobject.h>
#include <qproperty.h>
#include <qtmetamacros.h>

class BaseInput : public JsonFormItemWidget {
  Q_OBJECT

  QLineEdit *m_input = new QLineEdit(this);
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

  bool eventFilter(QObject *sender, QEvent *event) override;

protected:
  void showEvent(QShowEvent *event) override {
    recalculate();
    QWidget::showEvent(event);
  }

public:
  void setLeftAccessory(QWidget *widget);
  void setRightAccessory(QWidget *widget);
  FocusNotifier *focusNotifier() const override { return m_focusNotifier; }
  void clear();
  QString text() const { return m_input->text(); }
  void setText(const QString &text);
  void setPlaceholderText(const QString &text);
  void selectAll() { m_input->selectAll(); }
  void setEchoMode(QLineEdit::EchoMode echo) { m_input->setEchoMode(echo); }
  QLineEdit::EchoMode echoMode() { return m_input->echoMode(); }
  void setReadOnly(bool value);
  QLineEdit *input() const;

  BaseInput(QWidget *parent = nullptr);
  ~BaseInput();

signals:
  void textChanged(const QString &);
};
