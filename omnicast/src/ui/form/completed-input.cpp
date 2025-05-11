#include "ui/form/completed-input.hpp"
#include "ui/focus-notifier.hpp"
#include "ui/form/base-input.hpp"
#include <memory>
#include <qjsonvalue.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qwidget.h>

bool CompletedInput::eventFilter(QObject *obj, QEvent *event) {
  if (obj == inputField) {
    if (event->type() == QEvent::KeyPress) {
      auto kv = static_cast<QKeyEvent *>(event);

      if (kv->modifiers() == Qt::Modifiers{} && kv->key() == Qt::Key_Escape) {
        if (popover->isVisible()) {
          popover->close();
          return true;
        }
      }

      if (kv->modifiers() == Qt::Modifiers{} && kv->key() == Qt::Key_Return) {
        showPopover();
        return true;
      }
    }
  }

  return false;
}

QJsonValue CompletedInput::asJsonValue() const {
  return _currentSelection ? _currentSelection->id() : QJsonValue();
}

void CompletedInput::setIsLoading(bool value) { m_loadingBar->setStarted(value); }

void CompletedInput::setValueAsJson(const QJsonValue &value) { setText(value.toString()); }

FocusNotifier *CompletedInput::focusNotifier() const { return m_focusNotifier; }

CompletedInput::CompletedInput(QWidget *parent)
    : QWidget(parent), m_list(new OmniList), inputField(new BaseInput), popover(new Popover(this)),
      selectionIcon(new OmniIcon) {
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  m_loadingBar->setPositionStep(5);

  setFocusProxy(inputField);

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
  popoverLayout->setContentsMargins(1, 1, 1, 1);
  popoverLayout->setSpacing(0);

  popoverLayout->addWidget(m_loadingBar);

  inputField->installEventFilter(this);
  popover->installEventFilter(this);

  auto listContainerWidget = new QWidget;
  auto listContainerLayout = new QVBoxLayout;

  listContainerLayout->setContentsMargins(0, 0, 0, 0);
  listContainerLayout->addWidget(m_list);
  listContainerWidget->setLayout(listContainerLayout);

  popoverLayout->addWidget(listContainerWidget);

  popover->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);

  connect(m_list, &OmniList::itemActivated, this, &CompletedInput::itemActivated);
  connect(inputField, &BaseInput::textChanged, this, &CompletedInput::handleTextChanged);

  setLayout(layout);
}

void CompletedInput::handleTextChanged(const QString &text) {
  if (m_completer) {
    auto completions = m_completer->generateCompletions(text);

    if (!completions.empty()) {
      qDebug() << "Completion popup not yet implemented";
      showPopover();
    } else {
      popover->close();
    }
  }

  emit textChanged(text);
}

void CompletedInput::itemActivated(const OmniList::AbstractVirtualItem &vitem) {
  // TODO: insert completion
  popover->close();
  emit selectionChanged(static_cast<const AbstractItem &>(vitem));
}

void CompletedInput::setPlaceholderText(const QString &text) { inputField->setPlaceholderText(text); }

void CompletedInput::setCompleter(std::unique_ptr<Completer> completer) {
  m_completer = std::move(completer);
}

QString CompletedInput::text() const { return inputField->text(); }

void CompletedInput::setText(const QString &value) { inputField->setText(value); }

void CompletedInput::showPopover() {
  const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

  popover->move(globalPos);
  popover->resize(inputField->width(), POPOVER_HEIGHT);
  popover->show();
  inputField->setFocus();
}

CompletedInput::~CompletedInput() {
  qDebug() << "~FormDropdown2";
  popover->deleteLater();
}

void CompletedInput::clear() {
  inputField->clear();
  _currentSelection.reset();
}
