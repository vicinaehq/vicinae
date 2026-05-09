#pragma once
#include "services/keyboard/abstract-keyboard-service.hpp"
#include "services/paste/abstract-paste-service.hpp"

class LinuxPasteService : public AbstractPasteService {
public:
  explicit LinuxPasteService(AbstractKeyboardService &keyboard);

  bool supportsPaste() const override;
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                  const AbstractApplication *app) override;

private:
  AbstractKeyboardService &m_keyboard;
};
