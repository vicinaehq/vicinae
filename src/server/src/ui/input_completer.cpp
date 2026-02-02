#include "ui/input_completer.hpp"
#include "omni-icon.hpp"
#include <qnamespace.h>

InputCompleter::InputCompleter(const QList<QString> &placeholders, QWidget *parent)
    : QWidget(parent), icon(new OmniIcon) {
  auto mainContainer = new QHBoxLayout();

  icon->setFixedSize(25, 25);

  mainContainer->setAlignment(Qt::AlignLeft);
  mainContainer->setContentsMargins(0, 0, 0, 0);
  mainContainer->setSpacing(10);

  mainContainer->addWidget(icon, 0, Qt::AlignLeft);

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

void InputCompleter::setIcon(const OmniIconUrl &url) {
  // iconLabel->setPixmap(QIcon::fromTheme(iconName).pixmap(22, 22));
  icon->setUrl(url);
}
