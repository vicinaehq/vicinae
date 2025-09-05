#pragma once
#include "common.hpp"
#include "extend/action-model.hpp"
#include "../../../src/ui/image/url.hpp"
#include "ui/keyboard.hpp"
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qlogging.h>
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

  void setShortcut(const KeyboardShortcutModel &shortcut) { m_shortcuts = {shortcut}; }
  void addShortcut(const KeyboardShortcutModel &shortcut) { m_shortcuts.emplace_back(shortcut); }

  /**
   * First registered keyboard shortcut, sometimes referred as "primary" keyboard shortcut.
   */
  std::optional<KeyboardShortcutModel> shortcut() const {
    if (m_shortcuts.empty()) return std::nullopt;

    return m_shortcuts.front();
  }

  bool isBoundTo(const QKeyEvent *event) { return isBoundTo(KeyboardShortcut(event)); }
  bool isBoundTo(const KeyboardShortcutModel &model) { return std::ranges::contains(m_shortcuts, model); }
  bool isBoundTo(const KeyboardShortcut &shortcut) {
    return std::ranges::any_of(m_shortcuts,
                               [&](auto &&model) { return KeyboardShortcut(model) == shortcut; });
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
  virtual ImageURL icon() const { return m_icon; }

  AbstractAction() {}
  AbstractAction(const QString &title, const ImageURL &icon) : m_title(title), m_icon(icon) {}

  virtual void execute(ApplicationContext *context) {}

  void setAutoClose(bool value = true) { m_autoClose = value; }
  bool autoClose() const { return m_autoClose; }

  virtual bool isPushView() const { return false; }

  ~AbstractAction() {}

protected:
  QString m_title;
  ImageURL m_icon;
  Style m_style = Style::Normal;
  std::vector<KeyboardShortcutModel> m_shortcuts;
  bool m_primary = false;
  bool m_autoClose = false;

private:
  mutable QString m_id;
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

  void execute(ApplicationContext *ctx) override {
    if (m_fn) m_fn();
  }

public:
  SubmitAction(const std::function<void(void)> &fn) { m_fn = fn; }
};
