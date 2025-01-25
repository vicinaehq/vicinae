#pragma once
#include "omni-icon.hpp"
#include "ui/inline_qline_edit.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

class InputCompleter : public QWidget {

public:
  QList<InlineQLineEdit *> inputs;
  OmniIcon *icon;

  InputCompleter(const QList<QString> &placeholders, QWidget *parent = nullptr);

  void setIcon(const QString &iconName);

  QList<QString> collectArgs() const;

  // focus first empty placeholder
  // returns true if something was focused, false otherwise
  bool focusFirstEmpty() const;
};
