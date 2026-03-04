#pragma once
#include "linuxutils/keyboard.hpp"
#include "services/paste/abstract-paste-service.hpp"

class LinuxPasteService : public AbstractPasteService {
public:
  LinuxPasteService();

  bool supportsPaste() const override;
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                  const AbstractApplication *app) override;

private:
  linuxutils::UInputKeyboard m_keyboard;
};
