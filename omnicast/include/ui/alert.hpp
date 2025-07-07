#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/dialog.hpp"
#include "ui/omni-button.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qdir.h>
#include <qevent.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainterpath.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ButtonGroup : public QWidget {
public:
  ButtonGroup(OmniButtonWidget *left, OmniButtonWidget *right, QWidget *parent = nullptr) : QWidget(parent) {
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(left);
    layout->addWidget(right);
    setLayout(layout);
  }
};

class AlertWidget : public DialogContentWidget {
  Q_OBJECT

  OmniIcon *_icon;
  TypographyWidget *_title;
  TypographyWidget *_message;
  OmniButtonWidget *_cancelBtn;
  OmniButtonWidget *_actionBtn;
  std::function<void(void)> m_confirmCallback;
  std::function<void(void)> m_cancelCallback;

  void focusInEvent(QFocusEvent *event) override {
    _cancelBtn->setFocus();
    DialogContentWidget::focusInEvent(event);
  }

  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 6;
    QPainter painter(this);
    QPainterPath path;
    QPen pen(theme.colors.border, 1);

    painter.setRenderHint(QPainter::Antialiasing, true);
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor finalColor(theme.colors.statusBackground);

    finalColor.setAlphaF(0.98);
    painter.setPen(pen);
    painter.fillPath(path, finalColor);
    painter.drawPath(path);
  }

  void handleConfirm() {
    confirm();
    emit closeRequested();
  }

  void handleCancel() {
    canceled();
    emit closeRequested();
  }

protected:
  virtual void confirm() const {}
  virtual void canceled() const {}

public:
  void setTitle(const QString &title) { _title->setText(title); }
  void setConfirmCallback(const std::function<void(void)> &fn) {}
  void setCancelCallback(const std::function<void(void)> &fn) {}

  void setMessage(const QString &message) {
    _message->setText(message);
    _message->setVisible(!message.isEmpty());
  }

  void setCancelText(const QString &text, const ColorLike &color) {
    _cancelBtn->setText(text);
    _cancelBtn->setColor(color);
  }

  void setIcon(const std::optional<OmniIconUrl> &url) {
    if (!url) {
      _icon->hide();
      return;
    }

    _icon->setUrl(*url);
    _icon->show();
  }

  void setConfirmText(const QString &text, const ColorLike &color) {
    _actionBtn->setText(text);
    _actionBtn->setColor(color);
  }
  AlertWidget(QWidget *parent = nullptr)
      : DialogContentWidget(parent), _icon(new OmniIcon), _title(new TypographyWidget),
        _message(new TypographyWidget), _cancelBtn(new OmniButtonWidget), _actionBtn(new OmniButtonWidget) {
    auto layout = new QVBoxLayout;

    _message->setColor(ColorTint::TextSecondary);
    setFocusPolicy(Qt::StrongFocus);

    _icon->setFixedSize(25, 25);
    _icon->setUrl(BuiltinOmniIconUrl("trash").setFill(ColorTint::Red));
    _title->setText("Are you sure?");
    _title->setFontWeight(QFont::Bold);

    _message->setText("This action cannot be undone");
    _message->setWordWrap(true);

    QMargins btnMargins(50, 8, 50, 8);

    _cancelBtn->setFocus();
    _cancelBtn->setContentsMargins(btnMargins);
    _cancelBtn->setText("Cancel");
    _actionBtn->setContentsMargins(btnMargins);
    _actionBtn->setText("Delete");
    _actionBtn->setColor(ColorTint::Red);

    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);
    layout->addWidget(_icon, 0, Qt::AlignCenter);
    layout->addWidget(_title, 0, Qt::AlignCenter);
    layout->addWidget(_message, 0, Qt::AlignCenter);
    layout->addWidget(new ButtonGroup(_cancelBtn, _actionBtn));
    setLayout(layout);

    connect(_cancelBtn, &OmniButtonWidget::activated, this, &AlertWidget::handleCancel);
    connect(_actionBtn, &OmniButtonWidget::activated, this, &AlertWidget::handleConfirm);
  }

signals:
  void confirmed() const;
};

class CallbackAlertWidget : public AlertWidget {
  std::function<void(bool confirmed)> m_fn;

  void confirm() const override {
    if (m_fn) m_fn(true);
  }

  void canceled() const override {
    if (m_fn) m_fn(false);
  }

public:
  /**
   * Careful when using this inside the execute() method of an action.
   * There are rare scenarios in which the alert widget may live longer than the action it's triggered
   * in. To be on the safe side, make sure you do not capture reference to action members and do all the
   * capturing by value only.
   */
  void setCallback(const std::function<void(bool confirmed)> &fn) { m_fn = fn; }
};
