#pragma once
#include "abstract-window-manager.hpp"

class WindowManager {
  static std::vector<std::unique_ptr<AbstractWindowManager>> createCandidates();
  static std::unique_ptr<AbstractWindowManager> createProvider();

  std::unique_ptr<AbstractWindowManager> m_provider;

public:
  AbstractWindowManager *provider() const;
  AbstractWindowManager::WindowList listWindowsSync();
  AbstractWindowManager::WindowPtr getFocusedWindow();
  bool canPaste() const;

  WindowManager();
};
