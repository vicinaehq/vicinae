#pragma once

#include "argument.hpp"
#include "command-controller.hpp"
#include "command.hpp"
#include "common.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/dialog/dialog.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <chrono>
#include <google/protobuf/message.h>
#include <numeric>
#include <qevent.h>

class BaseView;
class DialogContentWidget;

#define VALUE_OR(VALUE, FALLBACK) (VALUE ? VALUE : FALLBACK)

struct ActionPanelSectionState {
  QString m_name;
  std::vector<std::shared_ptr<AbstractAction>> m_actions;

  auto actions() const { return m_actions; }
  QString name() const { return m_name; };
  void setName(const QString &text) { m_name = text; }
  void addAction(AbstractAction *action) { m_actions.emplace_back(action); }
};

// matches raycast pop to root type policiy
// https://developers.raycast.com/api-reference/window-and-search-bar#poptoroottype
enum class PopToRootType { Default, Immediate, Suspended };

struct CloseWindowOptions {
  PopToRootType popToRootType = PopToRootType::Default;
  bool clearRootSearch = false; // has no effect if we do not pop to root
};

struct PopToRootOptions {
  bool clearSearch = true;
};

class ActionPanelState : public NonCopyable {
public:
  enum class ShortcutPreset {
    None,
    List,
    Form,
  };

  AbstractAction *primaryAction() const { return m_primary; }

  /**
   * Apply shortcut presets and other things that need to be computed
   * at a given time.
   */
  void finalize() {
    computePrimaryAction();
    applyShortcuts();
  }

  const std::vector<std::unique_ptr<ActionPanelSectionState>> &sections() const { return m_sections; }

  ActionPanelSectionState *createSection(const QString &name = "") {
    auto section = std::make_unique<ActionPanelSectionState>();
    auto handle = section.get();

    section->setName(name);
    m_sections.emplace_back(std::move(section));

    return handle;
  }

  /**
   * The first action will be considered as the primary one, unless another
   * action was explicitly marked as primary.
   */
  void setAutoSelectPrimary(bool value = true) { m_autoSelectPrimary = value; }

  void setTitle(const QString &title) { m_title = title; }
  QString title() const { return m_title; }

  void setShortcutPreset(ShortcutPreset preset) { m_defaultShortcuts = shortcutsForPreset(preset); }

  ActionPanelState() { setShortcutPreset(ShortcutPreset::None); }

  int actionCount() const {
    auto acc = [](int acc, auto &&cur) { return acc + cur->actions().size(); };

    return std::accumulate(m_sections.begin(), m_sections.end(), 0, acc);
  }

private:
  void computePrimaryAction() {
    AbstractAction *first = nullptr;

    for (const auto &section : m_sections) {
      for (const auto &action : section->actions()) {
        if (!first) first = action.get();
        if (action->isPrimary()) {
          m_primarySection = section.get();
          m_primary = action.get();
          return;
        }
      }
    }

    if (m_autoSelectPrimary) {
      m_primary = first;
      if (first) { first->setPrimary(true); }
    }

    m_primarySection = !m_sections.empty() ? m_sections.front().get() : nullptr;
  }

  void applyShortcuts() {
    if (!m_primarySection) return;

    auto actions = m_primarySection->actions();

    for (int i = 0; i != actions.size() && i != m_defaultShortcuts.size(); ++i) {
      auto &action = actions[i];
      auto &shortcut = m_defaultShortcuts[i];
      auto existing = action->shortcut();

      // always prioritize default shortcut, but still keeps the one
      // that was set before as a secondary shortcut (usually not shown in UI).
      action->setShortcut(shortcut);
      if (existing) { action->addShortcut(*existing); }
    }
  }

  std::vector<Keyboard::Shortcut> shortcutsForPreset(ShortcutPreset preset) {
    switch (preset) {
    case ShortcutPreset::List:
      return {Keyboard::Shortcut::enter(), Keyboard::Shortcut::submit()};
    case ShortcutPreset::Form:
      return {Keyboard::Shortcut::submit()};
    default:
      break;
    }

    return {Keyboard::Shortcut::enter()};
  }

  bool m_autoSelectPrimary = true;
  QString m_title;
  std::vector<std::unique_ptr<ActionPanelSectionState>> m_sections;
  std::vector<Keyboard::Shortcut> m_defaultShortcuts;
  AbstractAction *m_primary = nullptr;
  ActionPanelSectionState *m_primarySection = nullptr;
};

class ListActionPanelState : public ActionPanelState {
public:
  ListActionPanelState() { setShortcutPreset(ShortcutPreset::List); }
};

class FormActionPanelState : public ActionPanelState {
public:
  FormActionPanelState() { setShortcutPreset(ShortcutPreset::Form); }
};

using ArgumentValues = std::vector<std::pair<QString, QString>>;

struct CompleterState {
  ArgumentList args;
  ArgumentValues values;
  ImageURL icon;

  CompleterState(const ArgumentList &args, const ImageURL &icon) : args(args), icon(icon) {}
};

struct GoBackOptions {
  bool ignoreInstantDismiss = false;
};

class NavigationController : public QObject, NonCopyable {
  Q_OBJECT

public:
  struct CommandFrame {
    QObjectUniquePtr<CommandContext> context;
    std::unique_ptr<CommandController> controller;

    std::shared_ptr<AbstractCmd> command;
    LaunchProps launchProps;
    size_t viewCount;

    ~CommandFrame() {
      context->unload();
      qDebug() << "Unloading command" << command->uniqueId();
    }
  };

  struct ViewState {
    BaseView *sender = nullptr;
    struct {
      QString title;
      ImageURL icon;
    } navigation;
    QString placeholderText;
    QString searchText;
    QObjectUniquePtr<QWidget> searchAccessory;
    bool accessoryVisibility = true;
    std::optional<CompleterState> completer;
    std::unique_ptr<ActionPanelState> actionPanelState;

    bool isLoading = false;
    bool supportsSearch = true;
    bool needsTopBar = true;
    bool needsStatusBar = true;
    bool panelOpened = false;

    ~ViewState();
  };

signals:
  void currentViewStateChanged(const ViewState &state) const;
  void currentViewChanged(const ViewState &state) const;
  void viewPushed(const BaseView *view);
  void viewPoped(const BaseView *view);
  void actionPanelVisibilityChanged(bool visible);
  void actionsChanged(const ActionPanelState &actions) const;
  void windowVisiblityChanged(bool visible);
  void searchTextSelected() const;
  void searchTextChanged(const QString &text) const;
  void searchPlaceholderTextChanged(const QString &text) const;
  void navigationStatusChanged(const QString &text, const ImageURL &icon) const;
  void navigationSuffixIconChanged(const std::optional<ImageURL> &icon) const;
  void confirmAlertRequested(DialogContentWidget *widget);
  void loadingChanged(bool value) const;
  void showHudRequested(const QString &title, const std::optional<ImageURL> &icon);

  void completionValuesChanged(const ArgumentValues &values) const;

  void invalidCompletionFired();

  void searchAccessoryChanged(QWidget *widget) const;
  void searchAccessoryCleared() const;
  void searchAccessoryVisiblityChanged(bool visible) const;

  void completionCreated(const CompleterState &completer) const;
  void completionDestroyed() const;

  void headerVisiblityChanged(bool value);
  void searchVisibilityChanged(bool value);
  void statusBarVisiblityChanged(bool value);
  void windowActivationChanged(bool value) const;

public:
  void closeWindow(const CloseWindowOptions &settings = {});
  void closeWindow(const CloseWindowOptions &settings, std::chrono::milliseconds delay);
  void showWindow();
  void toggleWindow();
  bool isWindowOpened() const;

  bool windowActivated();
  void setWindowActivated(bool value = true);

  void setPopToRootOnClose(bool value);

  bool hasCompleter() const;

  /**
   * If the instant dismiss flag is set to `true`, the next call to `goBack` or `closeWindow` will close the
   * window and pop to root regardless of the state of the navigation stack.
   *
   * This is typically set when opening a command using an external mechanism such as a deeplink.
   *
   * This flag automatically resets to false when consumed.
   */
  void setInstantDismiss(bool value = true);

  void setSearchPlaceholderText(const QString &text, const BaseView *caller = nullptr);
  void setSearchText(const QString &text, const BaseView *caller = nullptr);

  void setLoading(bool value, const BaseView *caller = nullptr);
  bool isLoading(const BaseView *caller = nullptr) const;

  void popToRoot(const PopToRootOptions &opts = {});

  QString searchText(const BaseView *caller = nullptr) const;
  QString navigationTitle(const BaseView *caller = nullptr) const;
  void searchPlaceholderText(const QString &text);

  AbstractAction *findBoundAction(const QKeyEvent *event) const;

  void setDialog(DialogContentWidget *dialog);
  void confirmAlert(const QString &title, const QString &description, const std::function<void()> &onConfirm);

  void createCompletion(const ArgumentList &args, const ImageURL &icon);
  void destroyCurrentCompletion();

  ArgumentValues completionValues() const;
  void setCompletionValues(const ArgumentValues &values);

  void selectSearchText() const;

  void openActionPanel();
  void closeActionPanel();
  void toggleActionPanel();

  void setActions(std::unique_ptr<ActionPanelState> state, const BaseView *caller = nullptr);
  void clearActions(const BaseView *caller = nullptr);

  void setSearchAccessory(QWidget *accessory, const BaseView *sender = nullptr);
  void clearSearchAccessory(const BaseView *sender = nullptr);

  void clearSearchText();
  void setNavigationTitle(const QString &navigationTitle, const BaseView *caller = nullptr);
  void setNavigationIcon(const ImageURL &icon);
  void setNavigationSuffixIcon(const std::optional<ImageURL> &icon);

  bool executePrimaryAction();
  void executeAction(AbstractAction *action);

  void setHeaderVisiblity(bool value, const BaseView *caller = nullptr);
  void setSearchVisibility(bool value, const BaseView *caller = nullptr);
  void setStatusBarVisibility(bool value, const BaseView *caller = nullptr);

  /**
   * Unlike clear accessory, this only alters the visiblity: doesn't delete anything.
   * We need to rework the search accessory system anyways, as the ownership rules
   * are beyond confusing.
   */
  void setSearchAccessoryVisibility(bool value, const BaseView *caller = nullptr);

  void showHud(const QString &title, const std::optional<ImageURL> &icon = std::nullopt);

  void launch(const std::shared_ptr<AbstractCmd> &cmd);
  void launch(const std::shared_ptr<AbstractCmd> &cmd, const ArgumentValues &arguments);
  void launch(const QString &id);
  const AbstractCmd *activeCommand() const;
  CommandFrame *activeFrame() const { return m_frames.back().get(); }
  bool reloadActiveCommand();
  void unloadActiveCommand();

  void setCloseOnFocusLoss(bool value);

  /**
   * Go back to previous navigation state.
   * If the `instantDismiss` flag is set, this will close the window and pop to root
   * no matter what and then clear the flag.
   * The flag can be completely bypassed by passing `ignoreInstantDismiss`.
   * See `setInstantDismiss`
   */
  void goBack(const GoBackOptions &opts = {});

  void popCurrentView();
  template <typename T> void pushView() { pushView(new T); }
  void pushView(BaseView *view);

  /**
   * Replace the current view without unloading the current command.
   * Can be useful to display introduction views before pushing the actual view.
   */
  void replaceView(BaseView *view);
  template <typename T> void replaceView() { replaceView(new T); }

  size_t viewStackSize() const;
  const ViewState *topState() const;
  ViewState *topState();

  NavigationController(ApplicationContext &ctx);

private:
  std::unique_ptr<ViewState> createViewState(BaseView *view) const;
  void activateView(const ViewState &state);

  ApplicationContext &m_ctx;
  std::vector<std::unique_ptr<CommandFrame>> m_frames;

  struct PendingPopToRoot {
    PopToRootType type = PopToRootType::Immediate;
    bool clearSearch = false;
  };

  void applyPopToRoot(const PendingPopToRoot &popToRoot);

  ViewState *findViewState(const BaseView *view);
  const ViewState *findViewState(const BaseView *view) const;
  const BaseView *topView() const;
  bool isRootSearch() const;

  bool m_windowOpened = false;
  bool m_windowActivated = false;
  bool m_isPanelOpened = false;
  bool m_popToRootOnClose = false;
  bool m_instantDismiss = false;
  bool m_closeOnFocusLoss = false;
  std::vector<std::unique_ptr<ViewState>> m_views;
  std::optional<PendingPopToRoot> m_pendingPopToRoot;
};
