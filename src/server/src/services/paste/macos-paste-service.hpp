#pragma once
#include "services/paste/abstract-paste-service.hpp"

class MacosPasteService : public AbstractPasteService {
public:
  bool supportsPaste() const override;
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                  const AbstractApplication *app) override;
};
