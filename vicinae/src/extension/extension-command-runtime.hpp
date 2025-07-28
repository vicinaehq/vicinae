#pragma once
#include "command.hpp"
#include "proto/extension.pb.h"

class ExtensionCommand;
class StorageRequestRouter;
class ExtensionNavigationController;
class UIRequestRouter;
class AppRequestRouter;
class ClipboardRequestRouter;
class ExtensionRequest;
class ExtensionEvent;

class ExtensionCommandRuntime : public CommandContext {
  std::shared_ptr<ExtensionCommand> m_command;

  std::unique_ptr<StorageRequestRouter> m_storageRouter;
  std::unique_ptr<ExtensionNavigationController> m_navigation;
  std::unique_ptr<UIRequestRouter> m_uiRouter;
  std::unique_ptr<AppRequestRouter> m_appRouter;
  std::unique_ptr<ClipboardRequestRouter> m_clipboardRouter;

  QString m_sessionId;

  proto::ext::extension::Response *makeErrorResponse(const QString &errorText);
  proto::ext::extension::Response *dispatchRequest(ExtensionRequest *request);
  void handleRequest(ExtensionRequest *request);
  void handleCrash(const proto::ext::extension::CrashEventData &crash);

  void handleOAuth(ExtensionRequest *request, const proto::ext::oauth::Request &req);

  void handleGenericEvent(const proto::ext::extension::GenericEventData &event) {}

  void handleEvent(const ExtensionEvent &event);
  void initialize();

public:
  void load(const LaunchProps &props) override;
  void unload() override;

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command);
};
