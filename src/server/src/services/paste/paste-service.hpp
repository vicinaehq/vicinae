#pragma once
#include <cstdint>
#include <memory>
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include "services/clipboard/clipboard-service.hpp"
#include "services/paste/abstract-paste-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/app-service/app-service.hpp"

class PasteService : public QObject {
  Q_OBJECT

public:
  enum class Target : std::uint8_t {
    // A Vicinae window is giving up focus, paste where it lands.
    FocusHandoff,
    // Paste into whatever has focus now, Vicinae windows included.
    CurrentFocus,
  };

  PasteService(ClipboardService &clipboard, WindowManager &wm, AppService &appDb,
               std::unique_ptr<AbstractPasteService> platform);

  bool pasteContent(const Clipboard::Content &content, const Clipboard::CopyOptions options = {},
                    Target target = Target::FocusHandoff);
  // Set on our own windows that are valid paste targets, unlike the launcher.
  static constexpr auto PASTE_TARGET_PROPERTY = "vicinaePasteTarget";

  bool supportsPaste() const;

private:
  void beginPaste();
  void waitForFocusAndPaste();
  void executePaste();

  ClipboardService &m_clipboard;
  WindowManager &m_wm;
  AppService &m_appDb;
  std::unique_ptr<AbstractPasteService> m_platform;

  Target m_target = Target::FocusHandoff;
  bool m_hasPendingPaste = false;
  bool m_awaitingCopy = false;
  QTimer m_focusPollTimer;
  QTimer m_copyWaitTimer;
  QElapsedTimer m_pasteTimer;
  int m_focusPollCount = 0;
};
