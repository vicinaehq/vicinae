#include "ui/top_bar.hpp"
#include "extend/image-model.hpp"
#include <qnamespace.h>

TopBar::TopBar(QWidget *parent) : QWidget(parent), layout(new QHBoxLayout()), input(new SearchBar()) {
  backButtonLabel = new QLabel();

  QIcon::setThemeName("Papirus-Dark");

  input->setTextMargins(10, 10, 10, 10);

  backWidget = new QWidget();
  auto backContainer = new QVBoxLayout();

  backContainer->setContentsMargins(0, 5, 0, 5);
  backContainer->addWidget(backButtonLabel, 0, Qt::AlignCenter);
  backWidget->setLayout(backContainer);

  backButtonLabel->setPixmap(QIcon::fromTheme("arrow-left").pixmap(20, 20));

  backButtonLabel->setProperty("class", "back-button");
  backWidget->hide();

  layout->addWidget(backWidget, 0, Qt::AlignLeft);
  layout->addWidget(input);
  layout->setSpacing(0);
  setLayout(layout);
  setProperty("class", "top-bar");
}

bool TopBar::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

    // forward completer return keys to main input
    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
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
}

void TopBar::activateQuicklinkCompleter(const CompleterData &data) {
  destroyQuicklinkCompleter();

  auto completion = new InputCompleter(data.placeholders);

  if (!data.placeholders.isEmpty()) {
    if (auto icon = std::get_if<ThemeIconModel>(&data.model)) { completion->setIcon(icon->iconName); }
  }

  for (const auto &input : completion->inputs) {
    input->installEventFilter(this);
  }

  quickInput = completion;
  auto fm = input->fontMetrics();
  input->setFixedWidth(fm.boundingRect(input->text()).width() + 35);
  layout->addWidget(completion, 1);
}

void TopBar::showBackButton() { backWidget->show(); }

void TopBar::hideBackButton() { backWidget->hide(); }
