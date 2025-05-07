#pragma once
#include "extend/action-model.hpp"
#include "omni-icon.hpp"
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

class AppWindow;

class AbstractAction : public QObject {
  Q_OBJECT

public:
  QString _title;
  OmniIconUrl iconUrl;
  std::optional<KeyboardShortcutModel> shortcut;
  std::function<void(void)> _execCallback;

  void setShortcut(const KeyboardShortcutModel &shortcut) { this->shortcut = shortcut; }
  void setExecutionCallback(const std::function<void(void)> &cb) { _execCallback = cb; }

  virtual QString id() const { return _title + iconUrl.toString(); }

  std::function<void(void)> executionCallback() const { return _execCallback; }

  virtual QString title() const { return _title; }

  AbstractAction(const QString &title, const OmniIconUrl &icon) : _title(title), iconUrl(icon) {}

  virtual void executePrelude(AppWindow &app);
  virtual void execute(AppWindow &app) = 0;

  virtual bool isPushView() const { return false; }

  ~AbstractAction() {}

signals:
  void didExecute();
};
