#pragma once
#include "services/paste/abstract-paste-service.hpp"

class MacosPasteService : public AbstractPasteService {
public:
  MacosPasteService();

  bool supportsPaste() const override;
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                  const AbstractApplication *app) override;
};
