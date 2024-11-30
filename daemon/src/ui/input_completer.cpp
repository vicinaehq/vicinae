#include "ui/input_completer.hpp"
#include <qnamespace.h>

InputCompleter::InputCompleter(const QList<QString> &placeholders,
                               QWidget *parent)
    : QWidget(parent) {
  auto mainContainer = new QHBoxLayout();

  QIcon::setThemeName("Papirus-Dark");

  setProperty("class", "quicklink-completion");

  mainContainer->setAlignment(Qt::AlignLeft);

  // mainContainer->setAlignment(Qt::AlignVCenter);
  mainContainer->setContentsMargins(0, 0, 0, 0);
  mainContainer->setSpacing(10);

  iconLabel = new QLabel();

  mainContainer->addWidget(iconLabel, 0, Qt::AlignLeft);

  for (const auto &placeholder : placeholders) {
    auto input = new InlineQLineEdit(placeholder);

    inputs.push_back(input);
    mainContainer->addWidget(input, 0, Qt::AlignLeft);
  }

  setLayout(mainContainer);
}

QList<QString> InputCompleter::collectArgs() const {
  QList<QString> ss;

  for (const auto &input : inputs)
    ss.push_back(input->text());

  return ss;
}

bool InputCompleter::focusFirstEmpty() const {
  for (auto &input : inputs) {
    if (input->text().isEmpty()) {
      emit input->setFocus();
      return true;
    }
  }

  return false;
}

void InputCompleter::setIcon(const QString &iconName) {
  iconLabel->setPixmap(QIcon::fromTheme(iconName).pixmap(22, 22));
}
