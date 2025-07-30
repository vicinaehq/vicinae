#pragma once

#include "ui/dialog.hpp"
#include "ui/image/omnimg.hpp"

class TypographyWidget;
class OmniButtonWidget;

class AlertWidget : public DialogContentWidget {
  Q_OBJECT

  Omnimg::ImageWidget *_icon;
  TypographyWidget *_title;
  TypographyWidget *_message;
  OmniButtonWidget *_cancelBtn;
  OmniButtonWidget *_actionBtn;
  std::function<void(void)> m_confirmCallback;
  std::function<void(void)> m_cancelCallback;

  void focusInEvent(QFocusEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void handleConfirm();
  void handleCancel();

protected:
  virtual void confirm() const;
  virtual void canceled() const;

public:
  void setTitle(const QString &title);
  void setMessage(const QString &message);
  void setCancelText(const QString &text, const ColorLike &color);
  void setIcon(const std::optional<OmniIconUrl> &url);
  void setConfirmText(const QString &text, const ColorLike &color);

  AlertWidget(QWidget *parent = nullptr);

signals:
  void confirmed() const;
};

class CallbackAlertWidget : public AlertWidget {
  std::function<void(bool confirmed)> m_fn;

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
};
