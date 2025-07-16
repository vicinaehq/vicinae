#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/top_bar.hpp"
#include <qlogging.h>
#include <qobject.h>
#include <qobjectdefs.h>

class BaseView;

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

class NavigationController : public QObject, NonCopyable {
  Q_OBJECT

public:
  struct ViewState {
    BaseView *sender = nullptr;
    struct {
      QString title;
      OmniIconUrl icon;
    } navigation;
    QString placeholderText;
    QString searchText;
    std::unique_ptr<QWidget> searchAccessory;
    struct {
      std::vector<std::pair<QString, QString>> values;
      std::optional<CompleterData> data;
    } completer;
    std::unique_ptr<ActionPanelState> actionPanelState;
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

  QString searchText(const BaseView *caller = nullptr) const;
  QString navigationTitle(const BaseView *caller = nullptr) const;
  void searchPlaceholderText(const QString &text);

  void selectSearchText() const;

  void openActionPanel();
  void closeActionPanel();

  void setActions(std::unique_ptr<ActionPanelState> state, const BaseView *caller = nullptr);

  void clearSearchText();
  void setNavigationTitle(const QString &navigationTitle, const BaseView *caller = nullptr);
  void setNavigationIcon(const OmniIconUrl &icon);

  bool executePrimaryAction();

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
  void navigationStatusChanged(const QString &text, const OmniIconUrl &icon) const;

private:
  ApplicationContext &m_ctx;

  void executeAction(AbstractAction *action);
  ViewState *findViewState(const BaseView *view);
  const ViewState *findViewState(const BaseView *view) const;
  const BaseView *topView() const;

  bool m_windowOpened = true;
  std::vector<std::unique_ptr<ViewState>> m_views;
};
