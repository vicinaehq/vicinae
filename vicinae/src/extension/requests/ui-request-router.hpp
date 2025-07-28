#pragma once
#include "extension/extension-navigation-controller.hpp"
#include "navigation-controller.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qobject.h>
#include "proto/ipc.pb.h"
#include "proto/ui.pb.h"
#include "services/toast/toast-service.hpp"
#include "ui/toast.hpp"

class UIRequestRouter : public QObject {
  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  ExtensionNavigationController *m_navigation = nullptr;
  ToastService &m_toast;

  ToastPriority parseProtoToastStyle(proto::ext::ui::ToastStyle style);

  proto::ext::ui::Response *showToast(const proto::ext::ui::ShowToastRequest &request);
  proto::ext::ui::Response *hideToast(const proto::ext::ui::HideToastRequest &request);
  proto::ext::ui::Response *updateToast(const proto::ext::ui::UpdateToastRequest &request);
  proto::ext::ui::Response *handleRender(const proto::ext::ui::RenderRequest &request);
  proto::ext::ui::Response *handleSetSearchText(const proto::ext::ui::SetSearchTextRequest &req);
  proto::ext::ui::Response *handleCloseWindow(const proto::ext::ui::CloseMainWindowRequest &req);
  proto::ext::ui::Response *pushView(const proto::ext::ui::PushViewRequest &req);
  proto::ext::ui::Response *popView(const proto::ext::ui::PopViewRequest &req);
  proto::ext::ui::Response *confirmAlert(const proto::ext::ui::ConfirmAlertRequest &req);
  proto::ext::ui::Response *showHud(const proto::ext::ui::ShowHudRequest &req);

  void modelCreated();

public:
  proto::ext::extension::Response *route(const proto::ext::ui::Request &req);

  UIRequestRouter(ExtensionNavigationController *navigation, ToastService &toast)
      : m_navigation(navigation), m_toast(toast) {
    connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this, &UIRequestRouter::modelCreated);
  }
};
