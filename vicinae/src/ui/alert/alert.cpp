#include "alert.hpp"
#include "theme.hpp"
#include "utils/layout.hpp"

void CallbackAlertWidget::confirm() const {
  if (m_fn) m_fn(true);
}

void CallbackAlertWidget::canceled() const {
  if (m_fn) m_fn(false);
}

void CallbackAlertWidget::setCallback(const std::function<void(bool confirmed)> &fn) { m_fn = fn; }

void AlertWidget::focusInEvent(QFocusEvent *event) {
  _cancelBtn->setFocus();
  DialogContentWidget::focusInEvent(event);
}

void AlertWidget::paintEvent(QPaintEvent *event) {
  auto &theme = ThemeService::instance().theme();
  int borderRadius = 6;
  QPainter painter(this);
  QPainterPath path;
  QPen pen(theme.colors.border, 2);

  painter.setRenderHint(QPainter::Antialiasing, true);
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor finalColor(theme.colors.statusBackground);

  finalColor.setAlphaF(0.98);
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

void AlertWidget::setIcon(const std::optional<OmniIconUrl> &url) {
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

void AlertWidget::setTitle(const QString &title) { _title->setText(title); }

AlertWidget::AlertWidget(QWidget *parent)
    : DialogContentWidget(parent), _icon(new Omnimg::ImageWidget), _title(new TypographyWidget),
      _message(new TypographyWidget), _cancelBtn(new OmniButtonWidget), _actionBtn(new OmniButtonWidget) {
  auto layout = new QVBoxLayout;

  _message->setColor(SemanticColor::TextSecondary);
  setFocusPolicy(Qt::StrongFocus);

  _icon->setFixedSize(25, 25);
  _icon->setUrl(BuiltinOmniIconUrl("trash").setFill(SemanticColor::Red));
  _title->setSize(TextSize::TextTitle);
  _title->setText("Are you sure?");
  _title->setFontWeight(QFont::Bold);

  _message->setText("This action cannot be undone");
  _message->setWordWrap(true);

  int btnHeight = 30;

  _cancelBtn->setFocus();
  _cancelBtn->setFixedHeight(btnHeight);
  _cancelBtn->setText("Cancel");
  _actionBtn->setFixedHeight(btnHeight);
  _actionBtn->setText("Delete");
  _actionBtn->setColor(SemanticColor::Red);

  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(10);
  layout->addWidget(_icon, 0, Qt::AlignCenter);
  layout->addWidget(_title, 0, Qt::AlignCenter);
  layout->addWidget(_message, 0, Qt::AlignCenter);
  layout->addWidget(HStack().add(_cancelBtn).add(_actionBtn).spacing(10).buildWidget());
  setLayout(layout);

  connect(_cancelBtn, &OmniButtonWidget::activated, this, &AlertWidget::handleCancel);
  connect(_actionBtn, &OmniButtonWidget::activated, this, &AlertWidget::handleConfirm);
}
