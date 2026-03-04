#pragma once
#include <memory>
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/abstract-paste-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/app-service/app-service.hpp"

class PasteService {
public:
  PasteService(ClipboardService &clipboard, WindowManager &wm, AppService &appDb,
               std::unique_ptr<AbstractPasteService> platform);

  bool pasteContent(const Clipboard::Content &content, const Clipboard::CopyOptions options = {});
  bool supportsPaste() const;

private:
  ClipboardService &m_clipboard;
  WindowManager &m_wm;
  AppService &m_appDb;
  std::unique_ptr<AbstractPasteService> m_platform;
};
