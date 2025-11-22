#pragma once
#include "common.hpp"
#include "ui/image/url.hpp"
#include "lib/keyboard/keyboard.hpp"
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qtmetamacros.h>

class AppWindow;
class ActionPanelView;

/**
 * The base class any action shown in the action panel inherits from.
 *
 * An action is often made of a title, icon, and one or many shortcuts bound to it.
 *
 * It must also implement the `execute` function that will execute the actual logic when
 * selected by the user as the action to run.
 * </br>
 * Note that although binding many shortcuts for a same action is possible, you should generally try to
 * avoid doing so for the sake of clarity.
 *
 * Also, note that only the first shortcut will be shown in the UI.
 */
class AbstractAction : public NonCopyable {
public:
  enum class Style { Normal, Danger };

  AbstractAction() {}
  AbstractAction(const QString &title, const std::optional<ImageURL> &icon) : m_title(title), m_icon(icon) {}
  ~AbstractAction() {}

  void setShortcut(const Keyboard::Shortcut &shortcut) { m_shortcuts = {shortcut}; }
  void addShortcut(const Keyboard::Shortcut &shortcut) { m_shortcuts.emplace_back(shortcut); }

  /**
   * First registered keyboard shortcut, sometimes referred as "primary" keyboard shortcut.
   */
  std::optional<Keyboard::Shortcut> shortcut() const {
    if (m_shortcuts.empty()) return std::nullopt;

    return m_shortcuts.front();
  }

  bool isBoundTo(const QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter) {
      qDebug() << "remapping numpad enter to return";
      return isBoundTo(Keyboard::Shortcut(Qt::Key_Return, event->modifiers() ^= Qt::KeypadModifier));
    }

    return isBoundTo(Keyboard::Shortcut(event));
  }

  bool isBoundTo(const Keyboard::Shortcut &shortcut) {
    return std::ranges::any_of(m_shortcuts, [&](auto &&model) { return model == shortcut; });
  }

  // Note: submenu are currently not implemented and a good implementation will require
  // good thinking on how to handle state (especially regarding extensions).
  virtual bool isSubmenu() const { return false; }
  virtual ActionPanelView *createSubmenu() const { return nullptr; }

  bool isPrimary() const { return m_primary; }

  void setPrimary(bool value) { m_primary = value; }
  void setStyle(AbstractAction::Style style) { m_style = style; }

  Style style() const { return m_style; }

  virtual QString id() const {
    if (m_id.isEmpty()) { m_id = QUuid::createUuid().toString(QUuid::WithoutBraces); }
    return m_id;
  }

  virtual QString title() const { return m_title; }
  virtual std::optional<ImageURL> icon() const { return m_icon; }

  virtual void execute(ApplicationContext *context) {}

  void setAutoClose(bool value = true) { m_autoClose = value; }
  bool autoClose() const { return m_autoClose; }

  virtual bool isPushView() const { return false; }

protected:
  QString m_title;
  std::optional<ImageURL> m_icon;
  Style m_style = Style::Normal;
  std::vector<Keyboard::Shortcut> m_shortcuts;
  bool m_primary = false;
  bool m_autoClose = false;

private:
  mutable QString m_id;
};

/**
 * An action that wraps another action, providing an `executeAfter` method
 * to execute logic after the wrapped action is done executing.
 */
class ProxyAction : public AbstractAction {
public:
  ProxyAction(AbstractAction *action) : m_proxy(action) {}

  void execute(ApplicationContext *context) override {
    m_proxy->execute(context);
    executeAfter(context);
  }

  virtual void executeAfter(ApplicationContext *ctx) {}

  QString title() const override { return m_proxy->title(); }
  std::optional<ImageURL> icon() const override { return m_proxy->icon(); }

private:
  std::unique_ptr<AbstractAction> m_proxy;
};

struct StaticAction : public AbstractAction {
  std::function<void(ApplicationContext *ctx)> m_fn;

  void execute(ApplicationContext *context) override {
    if (m_fn) m_fn(context);
  }

public:
  StaticAction(const QString &title, const std::optional<ImageURL> &url, const std::function<void()> &fn)
      : AbstractAction(title, url), m_fn([fn](ApplicationContext *ctx) { fn(); }) {}

  StaticAction(const QString &title, const std::optional<ImageURL> &url,
               const std::function<void(ApplicationContext *ctx)> &fn)
      : AbstractAction(title, url), m_fn(fn) {}
};

class SubmitAction : public AbstractAction {
  std::function<void(void)> m_fn;

  void execute(ApplicationContext *ctx) override {
    if (m_fn) m_fn();
  }

public:
  SubmitAction(const std::function<void(void)> &fn) { m_fn = fn; }
};
