#pragma once
#include "extend/action-model.hpp"
#include "omni-icon.hpp"
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

class AppWindow;

class AbstractAction : public QObject {
  Q_OBJECT

public:
  QString title;
  OmniIconUrl iconUrl;
  std::optional<KeyboardShortcutModel> shortcut;
  std::function<void(void)> _execCallback;

  void setShortcut(const KeyboardShortcutModel &shortcut) { this->shortcut = shortcut; }
  void setExecutionCallback(const std::function<void(void)> &cb) { _execCallback = cb; }

  std::function<void(void)> executionCallback() const { return _execCallback; }

  AbstractAction(const QString &title, const OmniIconUrl &icon) : title(title), iconUrl(icon) {}
  virtual void execute(AppWindow &app) = 0;

signals:
  void didExecute();
};
