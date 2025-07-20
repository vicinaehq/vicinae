#pragma once
#include "extension/extension-navigation-controller.hpp"
#include "navigation-controller.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qobject.h>
#include "proto/ipc.pb.h"

class UIRequestRouter : public QObject {
  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  ExtensionNavigationController *m_navigation = nullptr;

  proto::ext::ui::Response *handleRender(const proto::ext::ui::RenderRequest &request);
  proto::ext::ui::Response *handleSetSearchText(const proto::ext::ui::SetSearchTextRequest &req);
  proto::ext::ui::Response *handleCloseWindow(const proto::ext::ui::CloseMainWindowRequest &req);
  proto::ext::ui::Response *pushView(const proto::ext::ui::PushViewRequest &req);
  proto::ext::ui::Response *popView(const proto::ext::ui::PopViewRequest &req);
  void modelCreated();

public:
  proto::ext::extension::Response *route(const proto::ext::ui::Request &req);

  UIRequestRouter(ExtensionNavigationController *navigation) : m_navigation(navigation) {
    connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this, &UIRequestRouter::modelCreated);
  }
};
