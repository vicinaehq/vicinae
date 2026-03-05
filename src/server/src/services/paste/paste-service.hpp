#pragma once
#include <memory>
#include <QObject>
#include <QTimer>
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/abstract-paste-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/app-service/app-service.hpp"

class PasteService : public QObject {
  Q_OBJECT

public:
  PasteService(ClipboardService &clipboard, WindowManager &wm, AppService &appDb,
               std::unique_ptr<AbstractPasteService> platform);

  bool pasteContent(const Clipboard::Content &content, const Clipboard::CopyOptions options = {});
  bool supportsPaste() const;

private:
  void waitForFocusAndPaste();
  void executePaste();

  ClipboardService &m_clipboard;
  WindowManager &m_wm;
  AppService &m_appDb;
  std::unique_ptr<AbstractPasteService> m_platform;

  bool m_hasPendingPaste = false;
  QTimer m_focusPollTimer;
  int m_focusPollCount = 0;
};
