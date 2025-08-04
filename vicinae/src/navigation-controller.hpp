#pragma once

#include "common.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/dialog/dialog.hpp"
#include <QString>

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

struct ActionPanelState : public NonCopyable {
  AbstractAction *findPrimaryAction() const {
    for (const auto &section : m_sections) {
      for (const auto &action : section->actions()) {
        if (action->isPrimary()) return action.get();
      }
    }

    return nullptr;
  }

  QString m_title;
  std::vector<std::unique_ptr<ActionPanelSectionState>> m_sections;

  const std::vector<std::unique_ptr<ActionPanelSectionState>> &sections() const { return m_sections; }

  ActionPanelSectionState *createSection(const QString &name = "") {
    auto section = std::make_unique<ActionPanelSectionState>();
    auto handle = section.get();

    section->setName(name);
    m_sections.emplace_back(std::move(section));

    return handle;
  }

  void setTitle(const QString &title) { m_title = title; }
  QString title() const { return m_title; }
};

using ArgumentValues = std::pair<QString, QString>;

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

  void closeWindow();
  void showWindow();
  void toggleWindow();
  bool isWindowOpened() const;

  void setSearchPlaceholderText(const QString &text, const BaseView *caller = nullptr);
  void setSearchText(const QString &text, const BaseView *caller = nullptr);

  void setLoading(bool value, const BaseView *caller = nullptr);

  void popToRoot();

  QString searchText(const BaseView *caller = nullptr) const;
  QString navigationTitle(const BaseView *caller = nullptr) const;
  void searchPlaceholderText(const QString &text);

  void setDialog(DialogContentWidget *dialog);

  void createCompletion(const ArgumentList &args, const ImageURL &icon);
  void destroyCurrentCompletion();

  ArgumentValues completionValues() const;
  void setCompletionValues(const ArgumentValues &values);

  void selectSearchText() const;

  void openActionPanel();
  void closeActionPanel();

  void setActions(std::unique_ptr<ActionPanelState> state, const BaseView *caller = nullptr);
  void clearActions(const BaseView *caller = nullptr);

  void setSearchAccessory(QWidget *accessory, const BaseView *sender = nullptr);
  void clearSearchAccessory(const BaseView *sender = nullptr);

  void clearSearchText();
  void setNavigationTitle(const QString &navigationTitle, const BaseView *caller = nullptr);
  void setNavigationIcon(const ImageURL &icon);

  bool executePrimaryAction();

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
  void confirmAlertRequested(DialogContentWidget *widget);
  void loadingChanged(bool value) const;
  void showHudRequested(const QString &title, const std::optional<ImageURL> &icon);

  void searchAccessoryChanged(QWidget *widget) const;
  void searchAccessoryCleared() const;

  void completionCreated(const CompleterState &completer) const;
  void completionDestroyed() const;

  void headerVisiblityChanged(bool value);
  void searchVisibilityChanged(bool value);
  void statusBarVisiblityChanged(bool value);

private:
  ApplicationContext &m_ctx;

  void executeAction(AbstractAction *action);
  ViewState *findViewState(const BaseView *view);
  const ViewState *findViewState(const BaseView *view) const;
  const BaseView *topView() const;

  bool m_windowOpened = true;
  std::vector<std::unique_ptr<ViewState>> m_views;
};
