#pragma once
#include "command.hpp"
#include "common.hpp"
#include "proto/extension.pb.h"
#include "types.hpp"
#include <qfuturewatcher.h>

class ExtensionCommand;
class StorageRequestRouter;
class ExtensionNavigationController;
class UIRequestRouter;
class AppRequestRouter;
class ClipboardRequestRouter;
class ExtensionRequest;
class ExtensionEvent;
class FileSearchRequestRouter;
class WindowManagementRouter;
class CommandRequestRouter;
class OAuthRouter;

class ExtensionCommandRuntime : public CommandContext {

  proto::ext::extension::Response *makeErrorResponse(const QString &errorText);

  PromiseLike<proto::ext::extension::Response *> dispatchRequest(ExtensionRequest *request);

  void handleRequest(ExtensionRequest *request);
  void handleCrash(const proto::ext::extension::CrashEventData &crash);
  void handleGenericEvent(const proto::ext::extension::GenericEventData &event) {}
  void handleEvent(const ExtensionEvent &event);
  void initialize();

public:
  void load(const LaunchProps &props) override;
  void unload() override;

  ExtensionCommandRuntime(const std::shared_ptr<ExtensionCommand> &command);

private:
  using ResponseWatcher = QFutureWatcher<proto::ext::extension::Response *>;
  std::unordered_map<std::shared_ptr<ExtensionRequest>, std::shared_ptr<ResponseWatcher>> m_pendingFutures;

  std::shared_ptr<ExtensionCommand> m_command;
  std::unique_ptr<StorageRequestRouter> m_storageRouter;
  std::unique_ptr<ExtensionNavigationController> m_navigation;
  std::unique_ptr<UIRequestRouter> m_uiRouter;
  std::unique_ptr<AppRequestRouter> m_appRouter;
  std::unique_ptr<ClipboardRequestRouter> m_clipboardRouter;
  std::unique_ptr<FileSearchRequestRouter> m_fileSearchRouter;
  std::unique_ptr<WindowManagementRouter> m_wmRouter;
  std::unique_ptr<CommandRequestRouter> m_commandRouter;
  std::unique_ptr<OAuthRouter> m_oauthRouter;
  QString m_sessionId;
  bool m_isDevMode = false;
};
