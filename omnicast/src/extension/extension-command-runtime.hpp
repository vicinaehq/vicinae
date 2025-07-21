#pragma once
#include "command.hpp"
#include "common.hpp"
#include "extension-error-view.hpp"
#include "extension/extension-command.hpp"
#include "extension/extension-navigation-controller.hpp"
#include "extension/manager/extension-manager.hpp"
#include "extension/requests/app-request-router.hpp"
#include "extension/requests/clipboard-request-router.hpp"
#include "extension/requests/storage-request-router.hpp"
#include "extension/requests/ui-request-router.hpp"
#include "proto/extension.pb.h"
#include "timer.hpp"
#include <google/protobuf/struct.pb.h>
#include <memory>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qobject.h>
#include <qwidget.h>

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;
  Timer m_timer;

  std::unique_ptr<StorageRequestRouter> m_storageRouter;
  std::unique_ptr<ExtensionNavigationController> m_navigation;
  std::unique_ptr<UIRequestRouter> m_uiRouter;
  std::unique_ptr<AppRequestRouter> m_appRouter;
  std::unique_ptr<ClipboardRequestRouter> m_clipboardRouter;

  QString m_sessionId;

  proto::ext::extension::Response *makeErrorResponse(const QString &errorText);
  proto::ext::extension::Response *dispatchRequest(const ExtensionRequest &request);
  void handleRequest(ExtensionRequest &request);
  void handleCrash(const proto::ext::extension::CrashEventData &crash);

  void handleGenericEvent(const proto::ext::extension::GenericEventData &event) {}

  void handleEvent(const ExtensionEvent &event);
  void initialize();

public:
  void load(const LaunchProps &props) override;
  void unload() override;

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command);
};
