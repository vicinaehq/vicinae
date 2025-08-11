#include "ui/arg-completer/arg-completer.hpp"
#include "navigation-controller.hpp"
#include <ranges>

void ArgCompleter::clear() {
  m_args.clear();
  emit destroyed();
  hide();
}

void ArgCompleter::setIconUrl(const ImageURL &url) { m_icon->setUrl(url); }

void ArgCompleter::validate() {
  for (const auto &[arg, input] : std::views::zip(m_args, m_inputs)) {
    if (arg.required && input->text().isEmpty()) { input->setError("Required"); }
  }
}

void ArgCompleter::setValues(const ArgumentValues values) {
  for (const auto &[val, input] : std::views::zip(values, m_inputs)) {
    input->setText(val.second);
  }
}

void ArgCompleter::setArguments(const ArgumentList &args) {
  m_inputs.clear();

  while (m_layout->count() > 1) {
    auto item = m_layout->takeAt(1);

    if (auto w = item->widget()) w->deleteLater();
  }

  for (const auto &arg : args) {
    auto edit = new InlineQLineEdit(arg.placeholder, this);

    connect(edit, &InlineQLineEdit::textChanged, this, [this]() { emit valueChanged(collect()); });

    if (arg.type == CommandArgument::Password) edit->setEchoMode(QLineEdit::EchoMode::Password);

    m_inputs.emplace_back(edit);
    m_layout->addWidget(edit, 0, Qt::AlignLeft);
  }

  m_args = args;
  show();
  emit valueChanged(collect());
  emit activated();
}

ArgCompleter::ArgCompleter(QWidget *parent) : QWidget(parent) {
  m_icon->setFixedSize(25, 25);
  m_layout->setContentsMargins(0, 0, 0, 0);
  m_layout->addWidget(m_icon, 0);
  setLayout(m_layout);
}

std::vector<std::pair<QString, QString>> ArgCompleter::collect() {
  std::vector<std::pair<QString, QString>> items;

  items.reserve(m_args.size());

  for (int i = 0; i != m_args.size(); ++i) {
    items.push_back({m_args.at(i).name, m_inputs.at(i)->text()});
  }

  return items;
}
