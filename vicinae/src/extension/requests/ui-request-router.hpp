#pragma once
#include "extension/extension-navigation-controller.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qobject.h>
#include "navigation-controller.hpp"
#include "proto/extension.pb.h"
#include "proto/ui.pb.h"
#include "services/toast/toast-service.hpp"
#include "types.hpp"
#include "ui/toast/toast.hpp"

class UIRequestRouter : public QObject {
public:
  PromiseLike<proto::ext::extension::Response *> route(const proto::ext::ui::Request &req);
  UIRequestRouter(ExtensionNavigationController *navigation, ToastService &toast);

private:
  ToastStyle parseProtoToastStyle(proto::ext::ui::ToastStyle style);
  static PopToRootType parseProtoPopToRoot(proto::ext::ui::PopToRootType type);

  proto::ext::ui::Response *showToast(const proto::ext::ui::ShowToastRequest &request);
  proto::ext::ui::Response *hideToast(const proto::ext::ui::HideToastRequest &request);
  proto::ext::ui::Response *updateToast(const proto::ext::ui::UpdateToastRequest &request);
  proto::ext::ui::Response *handleRender(const proto::ext::ui::RenderRequest &request);
  proto::ext::ui::Response *handleSetSearchText(const proto::ext::ui::SetSearchTextRequest &req);
  proto::ext::ui::Response *handleCloseWindow(const proto::ext::ui::CloseMainWindowRequest &req);
  proto::ext::ui::Response *popToRoot(const proto::ext::ui::PopToRootRequest &req);
  proto::ext::ui::Response *pushView(const proto::ext::ui::PushViewRequest &req);
  proto::ext::ui::Response *popView(const proto::ext::ui::PopViewRequest &req);
  QFuture<proto::ext::extension::Response *> confirmAlert(const proto::ext::ui::ConfirmAlertRequest &req);
  proto::ext::ui::Response *showHud(const proto::ext::ui::ShowHudRequest &req);
  proto::ext::ui::Response *getSelectedText(const proto::ext::ui::GetSelectedTextRequest &req);
  void modelCreated();

  static proto::ext::extension::Response *wrapUI(proto::ext::ui::Response *uiRes);

  QFutureWatcher<ParsedRenderData> m_modelWatcher;
  ExtensionNavigationController *m_navigation = nullptr;
  ToastService &m_toast;
};
