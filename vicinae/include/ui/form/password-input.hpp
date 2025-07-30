#pragma once
#include "omni-icon.hpp"
#include "ui/form/base-input.hpp"
#include "ui/icon-button/icon-button.hpp"
#include <qlineedit.h>
#include <qwidget.h>

class PasswordInput : public BaseInput {
  IconButton *toggleVisiblityIcon = new IconButton();

  void setEchoMode(QLineEdit::EchoMode mode) {
    if (mode == QLineEdit::EchoMode::Password) {
      toggleVisiblityIcon->setUrl(BuiltinOmniIconUrl("eye"));
    } else if (mode == QLineEdit::EchoMode::Normal) {
      toggleVisiblityIcon->setUrl(BuiltinOmniIconUrl("eye-disabled"));
    }

    BaseInput::setEchoMode(mode);
  }

  void toggleVisiblity() {
    setEchoMode(echoMode() == QLineEdit::EchoMode::Password ? QLineEdit::EchoMode::Normal
                                                            : QLineEdit::EchoMode::Password);
  }

public:
  PasswordInput(QWidget *parent = nullptr) : BaseInput(parent) {
    toggleVisiblityIcon->setFixedSize(30, 30);
    toggleVisiblityIcon->setUrl(BuiltinOmniIconUrl("eye"));
    setEchoMode(QLineEdit::EchoMode::Password);
    setRightAccessory(toggleVisiblityIcon);
    connect(toggleVisiblityIcon, &IconButton::clicked, this, &PasswordInput::toggleVisiblity);
  }
};
