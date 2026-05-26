#pragma once
#include "services/paste/abstract-paste-service.hpp"

class LinuxInputServer;

class LinuxPasteService : public AbstractPasteService {
public:
  explicit LinuxPasteService(LinuxInputServer &server);

  bool supportsPaste() const override;
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                  const AbstractApplication *app) override;

private:
  LinuxInputServer &m_server;
};
