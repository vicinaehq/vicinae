#pragma once
#include "omni-icon.hpp"
#include "ui/form/base-input.hpp"
#include "ui/icon-button.hpp"
#include <qwidget.h>

class PasswordInput : public BaseInput {
  IconButton *toggleVisiblityIcon = new IconButton();

  void setEchoMode(EchoMode mode) {
    if (mode == EchoMode::Password) {
      toggleVisiblityIcon->setUrl(BuiltinOmniIconUrl("eye"));
    } else if (mode == EchoMode::Normal) {
      toggleVisiblityIcon->setUrl(BuiltinOmniIconUrl("eye-disabled"));
    }

    BaseInput::setEchoMode(mode);
  }

  void toggleVisiblity() {
    setEchoMode(echoMode() == EchoMode::Password ? EchoMode::Normal : EchoMode::Password);
  }

public:
  PasswordInput(QWidget *parent = nullptr) : BaseInput(parent) {
    toggleVisiblityIcon->setFixedSize(30, 30);
    toggleVisiblityIcon->setUrl(BuiltinOmniIconUrl("eye"));
    setEchoMode(EchoMode::Password);
    setRightAccessory(toggleVisiblityIcon);
    connect(toggleVisiblityIcon, &IconButton::clicked, this, &PasswordInput::toggleVisiblity);
  }
};
