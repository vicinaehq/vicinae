#pragma once
#include "linuxutils/keyboard.hpp"
#include "services/keyboard/abstract-keyboard-service.hpp"

class LinuxKeyboardService : public AbstractKeyboardService {
public:
  LinuxKeyboardService();

  bool supportsKeyInjection() const override;
  void paste(bool terminal) override;
  void backspace(int n) override;
  void moveCursorLeft(int n) override;
  void space() override;

private:
  linuxutils::UInputKeyboard m_keyboard;
};
