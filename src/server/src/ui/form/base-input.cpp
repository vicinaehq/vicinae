#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qjsonvalue.h>
#include <qstyle.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpainter.h>
#include "ui/form/base-input.hpp"
#include "theme.hpp"
#include "ui/image/image.hpp"

bool BaseInput::event(QEvent *event) { return QWidget::event(event); }

void BaseInput::resizeEvent(QResizeEvent *event) {
  JsonFormItemWidget::resizeEvent(event);
  recalculate();
}

void BaseInput::setRightIcon(const ImageURL &icon) {
  auto image = new ImageWidget;
  image->setUrl(icon);
  setRightAccessory(image);
}

void BaseInput::recalculate() {
  QMargins margins = m_defaultTextMargins;

  if (leftAccessory && leftAccessory->isVisible()) { margins.setLeft(leftAccessory->width() + 10); }

  if (rightAccessory && rightAccessory->isVisible()) { margins.setRight(rightAccessory->width() + 10); }

  m_input->setTextMargins(margins);

  if (leftAccessory && leftAccessory->isVisible()) {
    leftAccessory->hide();
    leftAccessory->show();
    leftAccessory->move(8, (height() - leftAccessory->height()) / 2);
  }

  if (rightAccessory && rightAccessory->isVisible()) {
    rightAccessory->move(width() - rightAccessory->width() - 8, (height() - rightAccessory->height()) / 2);
    rightAccessory->show();
  }

  update();
}

void BaseInput::setValueAsJson(const QJsonValue &value) { m_input->setText(value.toString()); }

void BaseInput::setLeftAccessory(QWidget *widget) {
  leftAccessory = widget;
  leftAccessory->setParent(this);
  leftAccessory->setFixedSize(18, 18);
}

void BaseInput::setRightAccessory(QWidget *widget) {
  rightAccessory = widget;
  rightAccessory->setParent(this);
  rightAccessory->setFixedSize(18, 18);
}

QJsonValue BaseInput::asJsonValue() const {
  return m_input->text().isEmpty() ? QJsonValue() : m_input->text();
}

void BaseInput::clear() { m_input->clear(); };

void BaseInput::setText(const QString &text) { m_input->setText(text); };

QLineEdit *BaseInput::input() const { return m_input; }

void BaseInput::setPlaceholderText(const QString &text) { m_input->setPlaceholderText(text); }

void BaseInput::setReadOnly(bool value) { m_input->setReadOnly(value); }

bool BaseInput::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_input) {
    switch (event->type()) {
    case QEvent::FocusIn:
      m_focusNotifier->focusChanged(true);
      break;
    case QEvent::FocusOut:
      m_focusNotifier->focusChanged(false);
      break;
    default:
      break;
    }
  }

  return JsonFormItemWidget::eventFilter(watched, event);
}

void BaseInput::setTextMargins(const QMargins &margins) { m_defaultTextMargins = margins; }

BaseInput::BaseInput(QWidget *parent) : leftAccessory(nullptr), rightAccessory(nullptr) {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_input);
  m_input->setFrame(false);
  m_input->installEventFilter(this);
  m_input->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
  setFocusProxy(m_input);
  setAttribute(Qt::WA_TranslucentBackground);
  m_input->setProperty("form-input", true); // used for border styling

  setStyleSheet(ThemeService::instance().inputStyleSheet());
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this,
          [this]() { setStyleSheet(ThemeService::instance().inputStyleSheet()); });

  connect(m_input, &QLineEdit::textChanged, this, &BaseInput::textChanged);
}

BaseInput::~BaseInput() {}
