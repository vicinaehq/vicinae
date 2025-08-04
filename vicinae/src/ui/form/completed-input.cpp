#include "ui/form/completed-input.hpp"
#include "common.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include "ui/image/image.hpp"
#include <memory>
#include <qjsonvalue.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qwidget.h>

bool CompletedInput::eventFilter(QObject *obj, QEvent *event) {
  if (obj == inputField) {
    if (event->type() == QEvent::KeyPress) {
      auto kv = static_cast<QKeyEvent *>(event);

      if (popover->isVisible()) {
        if (kv->modifiers() == Qt::Modifiers{}) {
          switch (kv->key()) {
          case Qt::Key_Escape: {
            if (popover->isVisible()) {
              popover->close();
              return true;
            }
          }
          case Qt::Key_Up:
          case Qt::Key_Down:
          case Qt::Key_Return:
          case Qt::Key_Enter:
            QApplication::sendEvent(m_completerList, event);
            return true;
          }
        }
      }
    }
  }

  return false;
}

QJsonValue CompletedInput::asJsonValue() const {
  return _currentSelection ? _currentSelection->generateId() : QJsonValue();
}

void CompletedInput::setValueAsJson(const QJsonValue &value) { setText(value.toString()); }

FocusNotifier *CompletedInput::focusNotifier() const { return m_focusNotifier; }

CompletedInput::CompletedInput(QWidget *parent)
    : JsonFormItemWidget(parent), m_completerList(new OmniList), inputField(new BaseInput),
      popover(new Popover(this)), selectionIcon(new ImageWidget) {
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  setFocusProxy(inputField->input());

  connect(inputField->focusNotifier(), &FocusNotifier::focusChanged, this, [this](bool value) {
    // we don't consider opening the selection menu a focus change
    if (!popover->isVisible()) {
      if (m_focused != value) {
        m_focusNotifier->focusChanged(value);
        m_focused = value;
      }
    }
  });

  popover->setProperty("class", "popover");

  // Main input field
  inputField->setPlaceholderText("Select an item...");
  inputField->setReadOnly(false); // Read-only to behave like a combo box

  layout->addWidget(inputField);

  // Create the popover
  popover->setWindowFlags(Qt::Popup);
  auto *popoverLayout = new QVBoxLayout(popover);
  popoverLayout->setContentsMargins(0, 0, 0, 0);
  popoverLayout->setSpacing(0);

  inputField->installEventFilter(this);
  popover->installEventFilter(this);

  auto listContainerWidget = new QWidget;
  auto listContainerLayout = new QVBoxLayout;

  listContainerLayout->setContentsMargins(0, 0, 0, 0);
  listContainerLayout->addWidget(m_completerList);
  listContainerWidget->setLayout(listContainerLayout);

  popoverLayout->addWidget(listContainerWidget);

  popover->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);

  connect(m_completerList, &OmniList::itemActivated, this, [this](const auto &item) {
    popover->close();
    emit completionActivated(item);
  });
  connect(m_completerList, &OmniList::virtualHeightChanged, this,
          [this](int height) { popover->setFixedHeight(height); });
  connect(inputField, &BaseInput::textChanged, this, [this](const QString &text) { emit textChanged(text); });

  setLayout(layout);
}

void CompletedInput::handleTextChanged(const QString &text) { emit textChanged(text); }

void CompletedInput::setPlaceholderText(const QString &text) { inputField->setPlaceholderText(text); }

void CompletedInput::setCompleter(std::unique_ptr<Completer> completer) {
  m_completer = std::move(completer);
}

QString CompletedInput::text() const { return inputField->text(); }

void CompletedInput::setText(const QString &value) { inputField->setText(value); }

void CompletedInput::showPopover() {
  if (m_completerList->virtualHeight() == 0) {
    popover->close();
    return;
  }

  const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

  popover->move(globalPos);
  popover->resize(inputField->width(), POPOVER_HEIGHT);
  popover->show();
  inputField->setFocus();
}

CompletedInput::~CompletedInput() { popover->deleteLater(); }

void CompletedInput::clear() {
  inputField->clear();
  _currentSelection.reset();
}
