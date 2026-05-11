#pragma once
#include "services/paste/abstract-paste-service.hpp"

class SnippetServer;

class LinuxPasteService : public AbstractPasteService {
public:
  explicit LinuxPasteService(SnippetServer &server);

  bool supportsPaste() const override;
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                  const AbstractApplication *app) override;

private:
  SnippetServer &m_server;
};
