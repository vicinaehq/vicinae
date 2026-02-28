#pragma once

#include "ui/dialog/dialog.hpp"
#include "ui/image/url.hpp"
#include "theme.hpp"
#include <functional>

class AlertWidget : public DialogContentWidget {
  Q_OBJECT

  std::function<void(void)> m_confirmCallback;
  std::function<void(void)> m_cancelCallback;
  bool m_finished = false;

  QString m_titleText = "Are you sure?";
  QString m_messageText = "This action cannot be undone";
  std::optional<ImageURL> m_iconUrl = ImageURL::builtin("warning").setFill(SemanticColor::Red);
  QString m_confirmText = "Confirm";
  QString m_cancelText = "Cancel";
  ColorLike m_confirmColor = SemanticColor::Red;
  ColorLike m_cancelColor = SemanticColor::Foreground;

  void handleConfirm();
  void handleCancel();

protected:
  virtual void confirm() const;
  virtual void canceled() const;
  void interrupted() override;

public:
  void setTitle(const QString &title);
  void setMessage(const QString &message);
  void setCancelText(const QString &text, const ColorLike &color);
  void setIcon(const std::optional<ImageURL> &url);
  void setConfirmText(const QString &text, const ColorLike &color);

  QString titleText() const { return m_titleText; }
  QString messageText() const { return m_messageText; }
  std::optional<ImageURL> iconUrl() const { return m_iconUrl; }
  QString confirmButtonText() const { return m_confirmText; }
  QString cancelButtonText() const { return m_cancelText; }
  ColorLike confirmColor() const { return m_confirmColor; }
  ColorLike cancelColor() const { return m_cancelColor; }

  void triggerConfirm();
  void triggerCancel();

  AlertWidget(QObject *parent = nullptr);

signals:
  void confirmed() const;
};

class CallbackAlertWidget : public AlertWidget {
  std::function<void(bool confirmed)> m_fn;
  std::function<void()> m_confirm;
  std::function<void()> m_cancel;

  void confirm() const override;
  void canceled() const override;

public:
  void setCallback(const std::function<void(bool confirmed)> &fn);
  void setConfirmCallback(const std::function<void()> &fn);
  void setCancelCallback(const std::function<void()> &fn);
};
