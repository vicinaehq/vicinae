#pragma once
#include "common.hpp"
#include "extend/action-model.hpp"
#include "../../../src/ui/image/url.hpp"
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qtmetamacros.h>

class AppWindow;
class ActionPanelView;

class AbstractAction : public QObject, public NonCopyable {
public:
  enum Style { Normal, Danger };

private:
  Q_OBJECT

  bool m_primary = false;

  Style m_style = Normal;

public:
  mutable QString m_id;
  QString _title;
  ImageURL iconUrl;
  std::optional<KeyboardShortcutModel> shortcut;
  std::function<void(void)> _execCallback;

  void setShortcut(const KeyboardShortcutModel &shortcut) { this->shortcut = shortcut; }
  void setExecutionCallback(const std::function<void(void)> &cb) { _execCallback = cb; }

  virtual bool isSubmenu() const { return false; }
  virtual ActionPanelView *createSubmenu() const { return nullptr; }

  /**
   * Whether this action is a candidate to be primary action.
   * The first primary action found (in order) is usually used as THE
   * primary action.
   */
  bool isPrimary() const { return m_primary; }

  void setPrimary(bool value) { m_primary = value; }
  void setStyle(AbstractAction::Style style) { m_style = style; }

  Style style() const { return m_style; }

  virtual QString id() const {
    if (m_id.isEmpty()) { m_id = QUuid::createUuid().toString(QUuid::WithoutBraces); }
    return m_id;
  }

  std::function<void(void)> executionCallback() const { return _execCallback; }

  virtual QString title() const { return _title; }
  virtual ImageURL icon() const { return iconUrl; }

  AbstractAction() {}
  AbstractAction(const QString &title, const ImageURL &icon) : _title(title), iconUrl(icon) {}

  virtual void execute(AppWindow &app) {}
  virtual void execute() { qWarning() << "Default execute"; }
  virtual void execute(ApplicationContext *context) {}

  virtual bool isPushView() const { return false; }

  ~AbstractAction() {}

signals:
  void didExecute();
};

struct StaticAction : public AbstractAction {
  std::function<void(ApplicationContext *ctx)> m_fn;

  void execute(ApplicationContext *context) override {
    if (m_fn) m_fn(context);
  }

public:
  StaticAction(const QString &title, const ImageURL &url, const std::function<void()> &fn)
      : AbstractAction(title, url), m_fn([fn](ApplicationContext *ctx) { fn(); }) {}

  StaticAction(const QString &title, const ImageURL &url,
               const std::function<void(ApplicationContext *ctx)> &fn)
      : AbstractAction(title, url), m_fn(fn) {}
};

class SubmitAction : public AbstractAction {
  std::function<void(void)> m_fn;

  void execute() override {
    if (m_fn) m_fn();
  }

public:
  SubmitAction(const std::function<void(void)> &fn) { m_fn = fn; }
};
