#include "ui/form/form-field.hpp"
#include "theme.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/typography.hpp"
#include <qnamespace.h>
#include <qwidget.h>

FormField::FormField(QWidget *widget, const QString &name)
    : _nameLabel(new TypographyWidget), _errorLabel(new TypographyWidget), _widget(widget),
      _layout(new QHBoxLayout) {
  setFocusPolicy(Qt::StrongFocus);
  _nameLabel->setText(name);
  _errorLabel->setColor(ColorTint::Red);
  _layout->setSpacing(20);
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->addWidget(_nameLabel, 2, Qt::AlignVCenter | Qt::AlignRight);
  _layout->addWidget(widget, 5, Qt::AlignVCenter);
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

  m_info->setDocumentMargin(0);
  m_info->setGrowAsRequired(true);
  m_info->setBaseTextColor(ColorTint::TextSecondary);
  m_mainLayout->addWidget(fieldRow);
  m_mainLayout->addWidget(m_infoContainer);
  m_mainLayout->setSpacing(10);
  m_infoContainer->hide();

  setLayout(m_mainLayout);
}

void FormField::setName(const QString &name) { _nameLabel->setText(name); }

void FormField::setInfo(const QString &info) {
  m_info->setMarkdown(info);
  m_infoContainer->setVisible(!info.isEmpty());
}

QString FormField::errorText() const { return _errorLabel->text(); }

void FormField::setError(const QString &error) { _errorLabel->setText(error); }

void FormField::clearError() { _errorLabel->clear(); }

bool FormField::hasError() const { return !_errorLabel->text().isEmpty(); }

void FormField::focus() const { _widget->setFocus(); }

QWidget *FormField::widget() const { return _widget; }

void FormField::setWidget(QWidget *widget, FocusNotifier *notifier) {
  auto current = _layout->itemAt(1)->widget();

  if (notifier) {
    connect(notifier, &FocusNotifier::focusChanged, this, [this](bool focus) {
      if (focus)
        emit focused();
      else
        emit blurred();
      emit focusChanged(focus);
    });
  }

  setFocusProxy(widget);
  _layout->replaceWidget(current, widget);
  current->deleteLater();
  _widget = widget;
}
