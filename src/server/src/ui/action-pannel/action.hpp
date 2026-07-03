#pragma once
#include "common/context.hpp"
#include "ui/image/url.hpp"
#include "internal/keyboard/keyboard.hpp"
#include <QUuid>
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <functional>
#include <memory>

class AppWindow;

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

  // simple wrapper that wraps the constructed action in a shared pointer.
  // the addAction overload that takes a raw pointer eventually converts them
  // to shared pointers.
  template <typename T, typename... Args> static std::shared_ptr<T> make(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  AbstractAction() = default;
  AbstractAction(const QString &title, const std::optional<ImageURL> &icon) : m_title(title), m_icon(icon) {}
  virtual ~AbstractAction() = default;

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
    // numpad enter is uniformized with the regular return key
    if (event->key() == Qt::Key_Enter) {
      return isBoundTo(Keyboard::Shortcut(Qt::Key_Return, event->modifiers()));
    }

    return isBoundTo(Keyboard::Shortcut(event));
  }

  bool isBoundTo(const Keyboard::Shortcut &shortcut) {
    return std::ranges::any_of(m_shortcuts, [&](auto &&model) { return model == shortcut; });
  }

  virtual bool isSubmenu() const { return false; }

  bool isPrimary() const { return m_primary; }

  void setPrimary(bool value) { m_primary = value; }
  void setStyle(AbstractAction::Style style) { m_style = style; }

  Style style() const { return m_style; }

  virtual QString id() const {
    if (m_id.isEmpty()) { m_id = QUuid::createUuid().toString(QUuid::WithoutBraces); }
    return m_id;
  }

  void setId(const QString &id) const { m_id = id; }

  virtual QString title() const { return m_title; }
  virtual std::optional<ImageURL> icon() const { return m_icon; }

  virtual void execute(ApplicationContext *) {}

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

  virtual void executeAfter(ApplicationContext *) {}

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
      : AbstractAction(title, url), m_fn([fn](ApplicationContext *) { fn(); }) {}

  StaticAction(const QString &title, const std::optional<ImageURL> &url,
               const std::function<void(ApplicationContext *ctx)> &fn)
      : AbstractAction(title, url), m_fn(fn) {}
};

class SubmitAction : public AbstractAction {
  std::function<void(void)> m_fn;

  void execute(ApplicationContext *) override {
    if (m_fn) m_fn();
  }

public:
  SubmitAction(const std::function<void(void)> &fn) : m_fn(fn) {}
};

class ActionPanelState;
class ActionPanelView;

/**
 * An action that opens a nested panel when activated instead of executing logic.
 *
 * Subclasses implement `createView` to produce the panel pushed onto the action
 * panel stack. `onOpen` is invoked every time the submenu is opened by the user.
 */
class SubmenuAction : public AbstractAction {
public:
  SubmenuAction(const QString &title, const std::optional<ImageURL> &icon) : AbstractAction(title, icon) {
    setAutoClose(false);
  }

  bool isSubmenu() const final { return true; }

  virtual void onOpen() {}
  virtual ActionPanelView *createView(QObject *parent) = 0;
};

/**
 * Convenience submenu whose panel is a plain action list described by an
 * `ActionPanelState`. Subclasses only implement `buildState`.
 */
class ListSubmenuAction : public SubmenuAction {
public:
  using SubmenuAction::SubmenuAction;

  virtual std::unique_ptr<ActionPanelState> buildState() const = 0;

  ActionPanelView *createView(QObject *parent) override;
};
