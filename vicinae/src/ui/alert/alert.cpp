#include "alert.hpp"
#include "theme.hpp"
#include "utils/layout.hpp"
#include <qnamespace.h>
#include <qpainterpath.h>

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

void AlertWidget::focusInEvent(QFocusEvent *event) {
  _cancelBtn->setFocus();
  DialogContentWidget::focusInEvent(event);
}

void AlertWidget::interrupted() { canceled(); }

void AlertWidget::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 6;
  QPainter painter(this);
  QPainterPath path;
  QPen pen(theme.resolveTint(SemanticColor::Border), 2);

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor finalColor(theme.resolveTint(SemanticColor::StatusBackground));
  painter.setPen(pen);
  painter.fillPath(path, finalColor);
  painter.drawPath(path);
}

void AlertWidget::handleConfirm() {
  confirm();
  emit closeRequested();
}

void AlertWidget::handleCancel() {
  canceled();
  emit closeRequested();
}

void AlertWidget::setMessage(const QString &message) {
  _message->setText(message);
  _message->setVisible(!message.isEmpty());
}

void AlertWidget::setCancelText(const QString &text, const ColorLike &color) {
  _cancelBtn->setText(text);
  _cancelBtn->setColor(color);
}

void AlertWidget::setIcon(const std::optional<ImageURL> &url) {
  if (!url) {
    _icon->hide();
    return;
  }

  _icon->setUrl(*url);
  _icon->show();
}

void AlertWidget::confirm() const {}
void AlertWidget::canceled() const {}

void AlertWidget::setConfirmText(const QString &text, const ColorLike &color) {
  _actionBtn->setText(text);
  _actionBtn->setColor(color);
}

void AlertWidget::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier) {
    switch (event->key()) {
    case Qt::Key_H:
      return _cancelBtn->setFocus();
    case Qt::Key_L:
      return _actionBtn->setFocus();
    }
  }

  if (event->modifiers().toInt() == 0) {
    switch (event->key()) {
    case Qt::Key_Left:
      return _cancelBtn->setFocus();
    case Qt::Key_Right:
      return _actionBtn->setFocus();
    }
  }

  DialogContentWidget::keyPressEvent(event);
}

void AlertWidget::setTitle(const QString &title) { _title->setText(title); }

AlertWidget::AlertWidget(QWidget *parent)
    : DialogContentWidget(parent), _icon(new ImageWidget), _title(new TypographyWidget),
      _message(new TypographyWidget), _cancelBtn(new OmniButtonWidget), _actionBtn(new OmniButtonWidget) {
  auto layout = new QVBoxLayout;

  _message->setColor(SemanticColor::TextSecondary);
  setFocusPolicy(Qt::StrongFocus);

  _icon->setFixedSize(25, 25);
  _icon->setUrl(ImageURL::builtin("warning").setFill(SemanticColor::Red));
  _title->setSize(TextSize::TextTitle);
  _title->setText("Are you sure?");
  _title->setFontWeight(QFont::Bold);

  _message->setText("This action cannot be undone");
  _message->setWordWrap(true);

  setMinimumWidth(400);

  int btnHeight = 30;

  _cancelBtn->setFocus();
  _cancelBtn->setFixedHeight(btnHeight);
  _cancelBtn->setText("Cancel");
  _actionBtn->setFixedHeight(btnHeight);
  _actionBtn->setText("Delete");
  _actionBtn->setColor(SemanticColor::ButtonDestructive);

  _title->setAlignment(Qt::AlignCenter);
  _message->setAlignment(Qt::AlignCenter);

  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(15);
  layout->addWidget(_icon, 0, Qt::AlignCenter);
  layout->addWidget(_title);
  layout->addWidget(_message);
  _message->setAlignment(Qt::AlignCenter);
  layout->addWidget(HStack().add(_cancelBtn).add(_actionBtn).spacing(10).buildWidget());
  setLayout(layout);

  connect(_cancelBtn, &OmniButtonWidget::activated, this, &AlertWidget::handleCancel);
  connect(_actionBtn, &OmniButtonWidget::activated, this, &AlertWidget::handleConfirm);
}
