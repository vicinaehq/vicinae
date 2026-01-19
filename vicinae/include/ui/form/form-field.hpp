#pragma once
#include "ui/focus-notifier.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include "ui/form/json-form-item-widget.hpp"
#include <qwidget.h>

class FormField : public QWidget {
public:
  using Validator = std::function<QString(const QJsonValue &value)>;

private:
  Q_OBJECT

  TypographyWidget *_nameLabel;
  TypographyWidget *_errorLabel;
  bool m_vertical = false;
  TypographyWidget *m_info = new TypographyWidget;
  JsonFormItemWidget *m_widget;
  QHBoxLayout *_layout;
  QWidget *m_infoContainer = new QWidget;
  QVBoxLayout *m_mainLayout = new QVBoxLayout(this);
  QHBoxLayout *m_infoLayout = new QHBoxLayout();
  Validator m_validator;

protected:
  void focusInEvent(QFocusEvent *event) override { QWidget::focusInEvent(event); }

  void focusOutEvent(QFocusEvent *event) override { QWidget::focusInEvent(event); }

  bool eventFilter(QObject *obj, QEvent *event) override;

public:
  FormField(QWidget *parent = nullptr);

  void setVerticalDirection(bool value);
  void setName(const QString &name);
  void setError(const QString &error);
  void setInfo(const QString &info);
  void setValidator(const Validator &validator);
  bool validate();
  QString errorText() const;
  void setWidget(JsonFormItemWidget *widget, FocusNotifier *focusNotifier = nullptr);
  void clearError();
  bool hasError() const;

  QWidget *widget() const;
  void focus() const;

signals:
  void focusChanged(bool value) const;
  void blurred() const;
  void focused() const;
};
