#pragma once
#include "common.hpp"
#include "extend/action-model.hpp"
#include "omni-icon.hpp"
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qtmetamacros.h>

class AppWindow;

class AbstractAction : public QObject, public NonAssignable {
  Q_OBJECT

  bool m_primary = false;

public:
  QString _title;
  OmniIconUrl iconUrl;
  std::optional<KeyboardShortcutModel> shortcut;
  std::function<void(void)> _execCallback;

  void setShortcut(const KeyboardShortcutModel &shortcut) { this->shortcut = shortcut; }
  void setExecutionCallback(const std::function<void(void)> &cb) { _execCallback = cb; }

  /**
   * Whether this action is a candidate to be primary action.
   * The first primary action found (in order) is usually used as THE
   * primary action.
   */
  bool isPrimary() const { return m_primary; }

  void setPrimary(bool value) { m_primary = value; }

  virtual QString id() const { return _title + iconUrl.toString(); }

  std::function<void(void)> executionCallback() const { return _execCallback; }

  virtual QString title() const { return _title; }

  AbstractAction(const QString &title, const OmniIconUrl &icon) : _title(title), iconUrl(icon) {}

  virtual void execute(AppWindow &app) {}
  virtual void execute() { qWarning() << "Default execute"; }

  virtual bool isPushView() const { return false; }

  ~AbstractAction() {}

signals:
  void didExecute();
};

struct StaticAction : public AbstractAction {
  std::function<void(void)> m_fn;

  void execute(AppWindow &app) override { m_fn(); }
  void execute() override { m_fn(); }

public:
  StaticAction(const QString &title, const OmniIconUrl &url, const std::function<void(void)> &fn)
      : AbstractAction(title, url), m_fn(fn) {}
};
