#pragma once
#include "services/paste/abstract-paste-service.hpp"

class DummyPasteService : public AbstractPasteService {
public:
  bool supportsPaste() const override { return false; }
  bool pasteToApp(const AbstractWindowManager::AbstractWindow *, const AbstractApplication *) override {
    return false;
  }
};
