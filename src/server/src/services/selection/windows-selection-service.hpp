#pragma once
#include "services/selection/abstract-selection-service.hpp"
#include <QObject>

class ClipboardService;
class WindowManager;

class WindowsSelectionService : public QObject, public AbstractSelectionService {
  Q_OBJECT

public:
  WindowsSelectionService(ClipboardService &clipboard, WindowManager &wm);

  QFuture<Result> selectedText() override;

  // true shortly after the foreground was lent to the target app for a copy chord
  static bool isLendingForeground();

private:
  QFuture<Result> copyFromTarget(void *target, void *focus);

  ClipboardService &m_clipboard;
  WindowManager &m_wm;
  bool m_copying = false;
};
