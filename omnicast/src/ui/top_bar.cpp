#include "ui/top_bar.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/icon-button.hpp"
#include <qnamespace.h>

TopBar::TopBar(QWidget *parent) : QWidget(parent), layout(new QHBoxLayout()), input(new SearchBar(this)) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  backButton = new IconButton;

  // input->setTextMargins(10, 10, 10, 10);

  backButton->setFixedSize(25, 25);

  connect(&ThemeService::instance(), &ThemeService::themeChanged, [this](const ThemeInfo &info) {
    backButton->setBackgroundColor(info.colors.mainSelectedBackground);
  });

  backButton->setUrl(BuiltinOmniIconUrl("arrow-left"));
  backButton->hide();

  connect(backButton, &IconButton::clicked, input, &SearchBar::pop);

  layout->setContentsMargins(15, 10, 15, 10);
  layout->addWidget(backButton, 0, Qt::AlignLeft | Qt::AlignVCenter);
  layout->addWidget(input);
  layout->setSpacing(10);
  setLayout(layout);

  setProperty("class", "top-bar");

  connect(input, &QLineEdit::textChanged, this, [this]() {
    if (completerData) {
      auto fm = input->fontMetrics();
      input->setFixedWidth(fm.boundingRect(input->text()).width() + 35);
    }
  });
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

void TopBar::showBackButton() { backButton->show(); }

void TopBar::hideBackButton() { backButton->hide(); }
