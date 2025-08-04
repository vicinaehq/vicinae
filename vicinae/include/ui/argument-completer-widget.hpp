#pragma once
#include "argument.hpp"
#include "omni-icon.hpp"
#include "ui/image/image.hpp"
#include "ui/inline-input/inline_qline_edit.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

struct ArgumentCompleterWidget : public QWidget {
  Q_OBJECT

public:
  ArgumentList m_args;
  QHBoxLayout *m_layout = new QHBoxLayout;
  ImageWidget *m_icon = new ImageWidget();
  std::vector<InlineQLineEdit *> m_inputs;

public:
  ArgumentCompleterWidget(QWidget *parent = nullptr) : QWidget(parent) {
    m_icon->setFixedSize(25, 25);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_icon, 0);
    setLayout(m_layout);
  }

  void clear() {
    m_args.clear();
    emit destroyed();
    hide();
  }

  void setIconUrl(const OmniIconUrl &url) { m_icon->setUrl(url); }
  void setArguments(const ArgumentList &args) {
    m_inputs.clear();

    while (m_layout->count() > 1) {
      auto item = m_layout->takeAt(1);

      if (auto w = item->widget()) w->deleteLater();
    }

    for (const auto &arg : args) {
      auto edit = new InlineQLineEdit(arg.placeholder, this);

      connect(edit, &InlineQLineEdit::textChanged, this, [this]() {
        qDebug() << "text changed";
        emit valueChanged(collect());
      });

      if (arg.type == CommandArgument::Password) edit->setEchoMode(QLineEdit::EchoMode::Password);

      m_inputs.emplace_back(edit);
      m_layout->addWidget(edit, 0, Qt::AlignLeft);
    }

    m_layout->addStretch();

    m_args = args;
    qDebug() << "showing arguments";
    show();
    emit activated();
  }

  std::vector<std::pair<QString, QString>> collect() {
    std::vector<std::pair<QString, QString>> items;

    items.reserve(m_args.size());

    for (int i = 0; i != m_args.size(); ++i) {
      items.push_back({m_args.at(i).name, m_inputs.at(i)->text()});
    }

    return items;
  }

signals:
  void activated() const;
  void destroyed() const;
  void valueChanged(const std::vector<std::pair<QString, QString>> &arguments);
};
