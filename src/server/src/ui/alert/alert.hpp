#pragma once

#include "ui/dialog/dialog.hpp"
#include "ui/image/image.hpp"
#include <functional>
#include <qevent.h>

class TypographyWidget;
class ButtonWidget;

class AlertWidget : public DialogContentWidget {
  Q_OBJECT

  ImageWidget *_icon;
  TypographyWidget *_title;
  TypographyWidget *_message;
  ButtonWidget *_cancelBtn;
  ButtonWidget *_actionBtn;
  std::function<void(void)> m_confirmCallback;
  std::function<void(void)> m_cancelCallback;
  bool m_finished = false;

  void focusInEvent(QFocusEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void handleConfirm();
  void handleCancel();

protected:
  virtual void confirm() const;
  virtual void canceled() const;
  void interrupted() override;

  void keyPressEvent(QKeyEvent *event) override;

public:
  void setTitle(const QString &title);
  void setMessage(const QString &message);
  void setCancelText(const QString &text, const ColorLike &color);
  void setIcon(const std::optional<ImageURL> &url);
  void setConfirmText(const QString &text, const ColorLike &color);

  AlertWidget(QWidget *parent = nullptr);

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
  /**
   * Careful when using this inside the execute() method of an action.
   * There are rare scenarios in which the alert widget may live longer than the action it's triggered
   * in. To be on the safe side, make sure you do not capture reference to action members and do all the
   * capturing by value only.
   */
  void setCallback(const std::function<void(bool confirmed)> &fn);
  void setConfirmCallback(const std::function<void()> &fn);
  void setCancelCallback(const std::function<void()> &fn);
};
