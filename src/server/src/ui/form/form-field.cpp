#include "theme.hpp"
#include "ui/form/form-field.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/typography/typography.hpp"
#include <qnamespace.h>

FormField::FormField(QWidget *parent)
    : QWidget(parent), _nameLabel(new TypographyWidget), _errorLabel(new TypographyWidget), m_widget(nullptr),
      _layout(new QHBoxLayout) {
  setFocusPolicy(Qt::StrongFocus);
  _nameLabel->setColor(SemanticColor::TextMuted);
  _errorLabel->setColor(SemanticColor::Red);
  _errorLabel->setSize(TextSize::TextSmaller);
  _layout->setSpacing(20);
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->addWidget(_nameLabel, 2, Qt::AlignVCenter | Qt::AlignRight);
  _layout->addWidget(new QWidget, 5, Qt::AlignVCenter);
  _layout->addWidget(_errorLabel, 2, Qt::AlignVCenter);

  // m_info->setWordWrap(true);

  auto fieldRow = new QWidget;

  fieldRow->setLayout(_layout);

  m_infoLayout->setContentsMargins(0, 0, 0, 0);
  m_infoLayout->addStretch(2);
  m_infoLayout->addSpacing(20);
  m_infoLayout->addWidget(m_info, 5);
  m_infoLayout->addSpacing(20);
  m_infoLayout->addStretch(2);
  m_infoContainer->setLayout(m_infoLayout);

  m_info->setColor(SemanticColor::TextMuted);
  m_info->setSize(TextSize::TextSmaller);
  m_info->setWordWrap(true);
  m_mainLayout->addWidget(fieldRow);
  m_mainLayout->addWidget(m_infoContainer);
  m_mainLayout->setSpacing(10);
  m_infoContainer->hide();

  setLayout(m_mainLayout);
}

void FormField::setVerticalDirection(bool value) {
  m_vertical = value;

  _errorLabel->setVisible(!value);

  if (value) {
    while (m_infoLayout->count() > 0) {
      m_infoLayout->takeAt(0);
    }

    m_infoLayout->addWidget(m_info);
  }

  _layout->setStretch(0, 0);
  _layout->itemAt(0)->setAlignment(Qt::AlignLeft);
  _layout->setStretch(1, 0);
  _layout->setStretch(2, 0);
  _layout->setDirection(value ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
}

void FormField::setName(const QString &name) { _nameLabel->setText(name); }

void FormField::setInfo(const QString &info) {
  m_info->setText(info);
  m_infoContainer->setVisible(!info.isEmpty());
}

void FormField::setValidator(const Validator &validator) { m_validator = validator; }

bool FormField::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::FocusIn) {
    emit focusChanged(true);
  } else if (event->type() == QEvent::FocusOut) {
    emit focusChanged(false);
  }

  return false;
}

bool FormField::validate() {
  if (m_validator) {
    if (auto error = m_validator(m_widget->asJsonValue()); !error.isEmpty()) {
      setError(error);
      return false;
    }
  }

  clearError();

  return true;
}

QString FormField::errorText() const { return _errorLabel->text(); }

void FormField::setError(const QString &error) { _errorLabel->setText(error); }

void FormField::clearError() { _errorLabel->clear(); }

bool FormField::hasError() const { return !_errorLabel->text().isEmpty(); }

void FormField::focus() const { m_widget->setFocus(); }

QWidget *FormField::widget() const { return m_widget; }

void FormField::setWidget(JsonFormItemWidget *widget, FocusNotifier *notifier) {
  auto current = _layout->itemAt(1)->widget();

  if (notifier) {
    connect(notifier, &FocusNotifier::focusChanged, this, [this](bool focus) {
      if (focus) {
        emit focused();
      } else {
        emit blurred();
        validate();
      }
      emit focusChanged(focus);
    });
  }

  setFocusProxy(widget);
  _layout->replaceWidget(current, widget);
  current->deleteLater();
  m_widget = widget;
}
