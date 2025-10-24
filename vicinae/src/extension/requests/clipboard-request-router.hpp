#pragma once
#include "extension/extension-navigation-controller.hpp"
#include "proto/clipboard.pb.h"
#include "proto/extension.pb.h"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include <qobject.h>

class ClipboardRequestRouter : public QObject {

public:
  proto::ext::extension::Response *route(const proto::ext::clipboard::Request &req);
  ClipboardRequestRouter(ClipboardService &clipboard, WindowManager &wm, const AppService &appDb,
                         ExtensionNavigationController &nav);

private:
  Clipboard::Content parseProtoClipboardContent(const proto::ext::clipboard::ClipboardContent &content);

  proto::ext::clipboard::Response *copy(const proto::ext::clipboard::CopyToClipboardRequest &req);
  proto::ext::clipboard::Response *paste(const proto::ext::clipboard::PasteToClipboardRequest &req);
  proto::ext::clipboard::Response *readContent(const proto::ext::clipboard::ReadContentRequest &req);
  proto::ext::clipboard::Response *clear(const proto::ext::clipboard::ClearRequest &req);

  ClipboardService &m_clipboard;
  WindowManager &m_wm;
  const AppService &m_appDb;
  ExtensionNavigationController &m_nav;
};
