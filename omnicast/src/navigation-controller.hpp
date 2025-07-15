#pragma once
#include "action-panel/action-panel.hpp"
#include "omni-icon.hpp"
#include "ui/top_bar.hpp"
#include <qlogging.h>
#include <qobject.h>
#include <qobjectdefs.h>

class BaseView;

#define VALUE_OR(VALUE, FALLBACK) (VALUE ? VALUE : FALLBACK)

class NavigationController : public QObject {
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
    ActionPanelV2Widget *actionPanel = nullptr;
    bool isLoading = false;
    bool supportsSearch = true;
    bool needsTopBar = true;
    bool needsStatusBar = true;

    ~ViewState();
  };

  void setSearchPlaceholderText(const QString &text);
  void setSearchText(const QString &text, const BaseView *caller = nullptr);

  QString searchText(const BaseView *caller = nullptr) const;
  void searchPlaceholderText(const QString &text);

  void clearSearchText();
  void setNavigationTitle(const QString &navigationTitle, const BaseView *caller = nullptr);
  void setNavigationIcon(const OmniIconUrl &icon);
  QString searchText() const;
  void popCurrentView();
  void pushView(BaseView *view);
  size_t viewStackSize() const;
  const ViewState *topState() const;
  ViewState *topState();

signals:
  void currentViewStateChanged(const ViewState &state) const;
  void currentViewChanged(const ViewState &state) const;
  void viewPushed(const BaseView *view);
  void viewPoped(const BaseView *view);

private:
  ViewState *findViewState(const BaseView *view);
  const ViewState *findViewState(const BaseView *view) const;
  const BaseView *topView() const;

  std::vector<std::unique_ptr<ViewState>> m_views;
};
