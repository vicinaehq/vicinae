#include "ui/form/form-field.hpp"
#include "theme.hpp"
#include "ui/typography.hpp"

FormField::FormField(QWidget *widget, const QString &name)
    : _nameLabel(new TypographyWidget), _errorLabel(new TypographyWidget), _widget(widget),
      _layout(new QHBoxLayout) {
  _nameLabel->setText(name);
  _errorLabel->setColor(ColorTint::Red);
  _layout->setSpacing(20);
  _layout->addWidget(_nameLabel, 1, Qt::AlignVCenter | Qt::AlignRight);
  _layout->addWidget(widget, 4, Qt::AlignVCenter);
  _layout->addWidget(_errorLabel, 2, Qt::AlignVCenter);
  setLayout(_layout);
}

void FormField::setName(const QString &name) { _nameLabel->setText(name); }

void FormField::setError(const QString &error) { _errorLabel->setText(error); }

void FormField::clearError() { _errorLabel->clear(); }

void FormField::focus() const { _widget->setFocus(); }

QWidget *FormField::widget() const { return _widget; }

void FormField::setWidget(QWidget *widget) {
  auto current = _layout->itemAt(1)->widget();

  _layout->replaceWidget(current, widget);
  current->deleteLater();
}
