#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

class AbstractPasteService {
public:
  virtual ~AbstractPasteService() = default;
  virtual bool supportsPaste() const = 0;
  virtual bool pasteToApp(const AbstractWindowManager::AbstractWindow *window,
                          const AbstractApplication *app) = 0;
};
