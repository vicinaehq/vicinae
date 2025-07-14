#pragma once

#include "base-view.hpp"
#include "common.hpp"
#include "omni-icon.hpp"
#include <qlogging.h>
#include <qobject.h>

class NavigationController : public QObject {
  Q_OBJECT

public:
  struct ViewState {
    QObjectUniquePtr<BaseView> sender;
    struct {
      QString title;
      OmniIconUrl icon;
    } navigation;
    QString placeholderText;
    QString text;
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
  };

  struct CommandFrame {
    std::unique_ptr<CommandContext> command;
    std::vector<ViewState> viewStack;

    CommandFrame(CommandContext *ctx) : command(ctx) {}
    ~CommandFrame() {
      qDebug() << "~CommandFrame - unloaded";
      command->unload();
    }
  };

  void setSearchPlaceholderText(const QString &text);
  void setSearchText(const QString &text);
  void setNavigationTitle(const QString &navigationTitle);
  void setNavigationIcon(const OmniIconUrl &icon);
  void popCurrentView() {}
  size_t viewStackSize() { return 1; }
  const ViewState *topState() const;
  ViewState *topState();

  NavigationController() { qCritical() << "NavigationController()"; }

signals:
  void currentViewStateChanged(const ViewState &state) const;
  void currentViewChanged(const ViewState &state) const;

private:
  std::vector<std::unique_ptr<CommandFrame>> m_commandFrames;
};
