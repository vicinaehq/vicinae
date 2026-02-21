#include "alert.hpp"

void CallbackAlertWidget::confirm() const {
  if (m_fn) m_fn(true);
  if (m_confirm) m_confirm();
}

void CallbackAlertWidget::canceled() const {
  if (m_fn) m_fn(false);
  if (m_cancel) m_cancel();
}

void CallbackAlertWidget::setCallback(const std::function<void(bool confirmed)> &fn) { m_fn = fn; }

void CallbackAlertWidget::setConfirmCallback(const std::function<void()> &fn) { m_confirm = fn; }

void CallbackAlertWidget::setCancelCallback(const std::function<void()> &fn) { m_cancel = fn; }

void AlertWidget::interrupted() {
  if (!m_finished) { canceled(); }
}

void AlertWidget::triggerConfirm() { handleConfirm(); }
void AlertWidget::triggerCancel() { handleCancel(); }

void AlertWidget::handleConfirm() {
  confirm();
  m_finished = true;
  emit closeRequested();
}

void AlertWidget::handleCancel() {
  canceled();
  m_finished = true;
  emit closeRequested();
}

void AlertWidget::setMessage(const QString &message) { m_messageText = message; }

void AlertWidget::setCancelText(const QString &text, const ColorLike &color) {
  m_cancelText = text;
  m_cancelColor = color;
}

void AlertWidget::setIcon(const std::optional<ImageURL> &url) { m_iconUrl = url; }

void AlertWidget::confirm() const {}
void AlertWidget::canceled() const {}

void AlertWidget::setConfirmText(const QString &text, const ColorLike &color) {
  m_confirmText = text;
  m_confirmColor = color;
}

void AlertWidget::setTitle(const QString &title) { m_titleText = title; }

AlertWidget::AlertWidget(QObject *parent) : DialogContentWidget(parent) {}
