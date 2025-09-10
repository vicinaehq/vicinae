#pragma once

#include "argument.hpp"
#include "common.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/dialog/dialog.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <google/protobuf/message.h>
#include <qevent.h>
#include <ranges>

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
  bool clearRootSearch = true; // has no effect if we do not pop to root
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

    return std::ranges::fold_left(m_sections, 0, acc);
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

    m_primary = m_autoSelectPrimary ? first : nullptr;
    m_primarySection = !m_sections.empty() ? m_sections.front().get() : nullptr;
  }

  void applyShortcuts() {
    if (!m_primarySection) return;

    auto actions = m_primarySection->actions();

    for (const auto &[action, shortcut] : std::views::zip(actions, m_defaultShortcuts)) {
      action->addShortcut(shortcut);
    }
  }

  std::vector<KeyboardShortcutModel> shortcutsForPreset(ShortcutPreset preset) {
    switch (preset) {
    case ShortcutPreset::List:
      return {KeyboardShortcutModel::enter(), KeyboardShortcutModel::submit()};
    case ShortcutPreset::Form:
      return {KeyboardShortcutModel::submit()};
    default:
      break;
    }

    return {KeyboardShortcutModel::enter()};
  }

  bool m_autoSelectPrimary = true;
  QString m_title;
  std::vector<std::unique_ptr<ActionPanelSectionState>> m_sections;
  std::vector<KeyboardShortcutModel> m_defaultShortcuts;
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

class NavigationController : public QObject, NonCopyable {
  Q_OBJECT

public:
  struct ViewState {
    BaseView *sender = nullptr;
    struct {
      QString title;
      ImageURL icon;
    } navigation;
    QString placeholderText;
    QString searchText;
    QObjectUniquePtr<QWidget> searchAccessory;
    std::optional<CompleterState> completer;
    std::unique_ptr<ActionPanelState> actionPanelState;
    bool loading;

    bool isLoading = false;
    bool supportsSearch = true;
    bool needsTopBar = true;
    bool needsStatusBar = true;
    bool panelOpened = false;

    ~ViewState();
  };

  bool m_isPanelOpened = false;
  bool m_popToRootOnClose = false;

  void closeWindow(const CloseWindowOptions &settings = {});
  void showWindow();
  void toggleWindow();
  bool isWindowOpened() const;

  void setPopToRootOnClose(bool value);

  void setSearchPlaceholderText(const QString &text, const BaseView *caller = nullptr);
  void setSearchText(const QString &text, const BaseView *caller = nullptr);

  void setLoading(bool value, const BaseView *caller = nullptr);

  void popToRoot(const PopToRootOptions &opts = {});

  QString searchText(const BaseView *caller = nullptr) const;
  QString navigationTitle(const BaseView *caller = nullptr) const;
  void searchPlaceholderText(const QString &text);

  AbstractAction *findBoundAction(const QKeyEvent *event) const;

  void setDialog(DialogContentWidget *dialog);

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

  void showHud(const QString &title, const std::optional<ImageURL> &icon = std::nullopt);

  void popCurrentView();
  void pushView(BaseView *view);
  size_t viewStackSize() const;
  const ViewState *topState() const;
  ViewState *topState();

  NavigationController(ApplicationContext &ctx);

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

  void completionCreated(const CompleterState &completer) const;
  void completionDestroyed() const;

  void headerVisiblityChanged(bool value);
  void searchVisibilityChanged(bool value);
  void statusBarVisiblityChanged(bool value);

private:
  ApplicationContext &m_ctx;

  ViewState *findViewState(const BaseView *view);
  const ViewState *findViewState(const BaseView *view) const;
  const BaseView *topView() const;
  bool isRootSearch() const;

  bool m_windowOpened = false;
  std::vector<std::unique_ptr<ViewState>> m_views;
};
