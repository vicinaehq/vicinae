#pragma once
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/dialog.hpp"
#include "ui/omni-button.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qdir.h>
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
      : DialogContentWidget(parent), _icon(new OmniIcon),
        _title(new TypographyWidget(TextSize::TextRegular, ColorTint::TextPrimary)),
        _message(new TypographyWidget(TextSize::TextRegular, ColorTint::TextSecondary)),
        _cancelBtn(new OmniButtonWidget), _actionBtn(new OmniButtonWidget) {
    auto layout = new QVBoxLayout;

    _icon->setFixedSize(25, 25);
    _icon->setUrl(BuiltinOmniIconUrl("trash").setFill(ColorTint::Red));
    _title->setText("Are you sure?");
    _title->setFontWeight(QFont::Bold);

    _message->setText("This action cannot be undone");

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
