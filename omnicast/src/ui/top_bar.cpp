#include "ui/top_bar.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/icon-button.hpp"
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>

TopBar::TopBar(QWidget *parent) : QWidget(parent), layout(new QHBoxLayout()), input(new SearchBar(this)) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  backButton = new IconButton;

  backButton->setFixedSize(25, 25);

  connect(&ThemeService::instance(), &ThemeService::themeChanged, this,
          [this](const ThemeInfo &info) { backButton->setBackgroundColor(info.colors.statusBackground); });

  backButton->setBackgroundColor(ThemeService::instance().theme().colors.statusBackground);
  backButton->setUrl(BuiltinOmniIconUrl("arrow-left"));
  backButton->hide();

  m_backButtonSpacer->setFixedWidth(5);
  m_backButtonSpacer->hide();

  connect(backButton, &IconButton::clicked, input, &SearchBar::pop);

  layout->setSpacing(0);
  layout->setContentsMargins(15, 10, 15, 10);
  layout->addWidget(backButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
  layout->addWidget(m_backButtonSpacer);
  layout->addWidget(input);
  layout->addSpacing(5);
  layout->addWidget(m_completer);
  // layout->setSpacing(10);
  layout->addWidget(m_accessory, 0, Qt::AlignRight | Qt::AlignVCenter);

  m_completer->hide();

  input->installEventFilter(this);

  setLayout(layout);

  setProperty("class", "top-bar");

  connect(input, &QLineEdit::textChanged, this, [this]() {
    if (m_completer->isVisible()) { input->setInline(true); }
  });
}

void TopBar::setAccessoryWidget(QWidget *accessory) {
  accessory->show();
  layout->replaceWidget(m_accessory, accessory);
  m_accessory = accessory;
}

void TopBar::clearAccessoryWidget() {
  auto widget = new QWidget();

  m_accessory->hide();
  layout->replaceWidget(m_accessory, widget);
  m_accessory = widget;
}

QWidget *TopBar::accessoryWidget() const { return m_accessory; }

bool TopBar::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

    // forward completer return keys to main input
    if ((key == Qt::Key_Up || key == Qt::Key_Down) && obj != input) {
      QApplication::sendEvent(input, event);
      return true;
    }

    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
      if (m_completer->isVisible()) {
        for (int i = 0; i != m_completer->m_args.size(); ++i) {
          auto &arg = m_completer->m_args.at(i);
          auto input = m_completer->m_inputs.at(i);

          qCritical() << "required" << arg.required << input->text();

          if (arg.required && input->text().isEmpty()) {
            input->setFocus();
            return true;
          }
        }
      }

      // do not send a new event to the input itself!
      if (obj == input) { return false; }

      QApplication::sendEvent(input, event);
      return true;
    }
  }

  return false;
}

void TopBar::destroyQuicklinkCompleter() {
  if (quickInput) {
    layout->removeWidget(quickInput);
    quickInput->deleteLater();
    quickInput = nullptr;
    input->setFocus();
    input->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
  }

  completerData.reset();
}

void TopBar::activateQuicklinkCompleter(const CompleterData &data) {
  destroyQuicklinkCompleter();

  completerData = data;

  auto completion = new InputCompleter(data.placeholders);

  if (!data.placeholders.isEmpty()) { completion->setIcon(data.iconUrl); }

  for (size_t i = 0; i != completion->inputs.size(); ++i) {
    auto input = completion->inputs.at(i);

    input->installEventFilter(this);
    if (i < data.values.size()) input->setText(data.values.at(i));
  }

  quickInput = completion;
  auto fm = input->fontMetrics();
  input->setFixedWidth(fm.boundingRect(input->text()).width() + 35);
  layout->addWidget(completion, 1);
}

void TopBar::showBackButton() {
  backButton->show();
  m_backButtonSpacer->show();
}

void TopBar::setBackButtonVisiblity(bool value) {
  backButton->setVisible(value);
  m_backButtonSpacer->setVisible(value);
}

void TopBar::hideBackButton() {
  backButton->hide();
  m_backButtonSpacer->hide();
}
