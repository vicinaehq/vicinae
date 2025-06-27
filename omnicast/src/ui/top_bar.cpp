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

  m_loadingBar->setFixedHeight(1);
  m_loadingBar->setBarWidth(100);

  layout->setSpacing(0);
  layout->setContentsMargins(15, 10, 15, 10);
  layout->addWidget(backButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
  layout->addWidget(m_backButtonSpacer);
  layout->addWidget(input);
  layout->addSpacing(5);
  layout->addWidget(m_completer);
  layout->addWidget(m_accessoryContainer, 0, Qt::AlignRight | Qt::AlignVCenter);

  m_completer->hide();

  input->installEventFilter(this);

  auto horizontal = new QWidget;

  horizontal->setLayout(layout);
  m_vlayout->setContentsMargins(0, 0, 0, 0);
  m_vlayout->setSpacing(0);
  m_vlayout->addWidget(horizontal);
  m_vlayout->addWidget(m_loadingBar);

  setLayout(m_vlayout);

  setProperty("class", "top-bar");

  connect(input, &QLineEdit::textChanged, this, [this]() {
    if (m_completer->isVisible()) { input->setInline(true); }
  });
}

void TopBar::setAccessoryWidget(QWidget *accessory) {
  /*
if (auto current = m_accessoryContainer->widget(0)) { m_accessoryContainer->removeWidget(current); }

m_accessoryContainer->addWidget(accessory);
m_accessoryContainer->setCurrentWidget(accessory);
m_accessory = accessory;
*/
}

void TopBar::clearAccessoryWidget() {
  /*
auto widget = new QWidget();
m_accessoryContainer->removeWidget(m_accessory);
m_accessory->hide();
setAccessoryWidget(widget);
*/
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
      /*
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
      */

      // do not send a new event to the input itself!
      if (obj == input) { return false; }

      QApplication::sendEvent(input, event);
      return true;
    }
  }

  return false;
}

void TopBar::setLoading(bool value) { m_loadingBar->setStarted(value); }

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
