#include "ui-request-router.hpp"
#include "ui/image/proto-url.hpp"
#include "navigation-controller.hpp"
#include "proto/extension.pb.h"
#include "proto/ui.pb.h"
#include "theme.hpp"
#include "timer.hpp"
#include "ui/alert/alert.hpp"
#include "ui/toast/toast.hpp"
#include <QtConcurrent/QtConcurrent>
#include <QClipboard>
#include <qfuturewatcher.h>
#include <unordered_map>

namespace ui = proto::ext::ui;

static const std::unordered_map<ui::ToastStyle, ToastStyle> toastMap = {
    {ui::ToastStyle::Success, ToastStyle::Success}, {ui::ToastStyle::Info, ToastStyle::Info},
    {ui::ToastStyle::Warning, ToastStyle::Warning}, {ui::ToastStyle::Error, ToastStyle::Danger},
    {ui::ToastStyle::Dynamic, ToastStyle::Dynamic},
};

UIRequestRouter::UIRequestRouter(ExtensionNavigationController *navigation, ToastService &toast)
    : m_navigation(navigation), m_toast(toast) {
  connect(&m_modelWatcher, &QFutureWatcher<RenderModel>::finished, this, &UIRequestRouter::modelCreated);
}

PromiseLike<proto::ext::extension::Response *> UIRequestRouter::route(const proto::ext::ui::Request &req) {
  using Request = proto::ext::ui::Request;

  switch (req.payload_case()) {
  case Request::kRender:
    return wrapUI(handleRender(req.render()));
  case Request::kSetSearchText:
    return wrapUI(handleSetSearchText(req.set_search_text()));
  case Request::kCloseMainWindow:
    return wrapUI(handleCloseWindow(req.close_main_window()));
  case Request::kPushView:
    return wrapUI(pushView(req.push_view()));
  case Request::kPopView:
    return wrapUI(popView(req.pop_view()));
  case Request::kShowToast:
    return wrapUI(showToast(req.show_toast()));
  case Request::kHideToast:
    return wrapUI(hideToast(req.hide_toast()));
  case Request::kUpdateToast:
    return wrapUI(updateToast(req.update_toast()));
  case Request::kConfirmAlert:
    return confirmAlert(req.confirm_alert());
  case Request::kGetSelectedText:
    return wrapUI(getSelectedText(req.get_selected_text()));
  case Request::kPopToRoot:
    return wrapUI(popToRoot(req.pop_to_root()));
  case Request::kShowHud:
    return wrapUI(showHud(req.show_hud()));

  default:
    break;
  }

  return nullptr;
  // return makeErrorResponse("Unhandled UI request");
}

ToastStyle UIRequestRouter::parseProtoToastStyle(ui::ToastStyle style) {
  if (auto it = toastMap.find(style); it != toastMap.end()) return it->second;

  return ToastStyle::Success;
}

void UIRequestRouter::modelCreated() {
  if (m_modelWatcher.isCanceled()) return;

  auto views = m_navigation->views();
  auto models = m_modelWatcher.result();

  for (int i = 0; i < models.items.size() && i < views.size(); ++i) {
    auto &model = models.items[i];
    auto &view = views[i];
    bool shouldSkipRender = !model.dirty && !model.propsDirty;

    if (!shouldSkipRender) view->render(model.root);
  }
}

proto::ext::ui::Response *UIRequestRouter::showToast(const proto::ext::ui::ShowToastRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;
  auto style = parseProtoToastStyle(req.style());

  m_toast.setToast(req.title().c_str(), style, req.message().c_str());
  res->set_allocated_show_toast(ack);

  return res;
}

PopToRootType UIRequestRouter::parseProtoPopToRoot(proto::ext::ui::PopToRootType type) {
  using Type = proto::ext::ui::PopToRootType;

  switch (type) {
  case Type::PopToRootImmediate:
    return PopToRootType::Immediate;
  case Type::PopToRootSuspended:
    return PopToRootType::Suspended;
  default:
    return PopToRootType::Default;
  }
}

proto::ext::ui::Response *UIRequestRouter::showHud(const proto::ext::ui::ShowHudRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  res->set_allocated_show_hud(ack);
  m_navigation->handle()->closeWindow({
      .popToRootType = parseProtoPopToRoot(req.pop_to_root()),
      .clearRootSearch = req.clear_root_search(),
  });
  m_navigation->handle()->showHud(req.text().c_str());

  return res;
}

proto::ext::ui::Response *UIRequestRouter::hideToast(const proto::ext::ui::HideToastRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  m_toast.clear();
  res->set_allocated_hide_toast(ack);

  return res;
}

proto::ext::ui::Response *UIRequestRouter::updateToast(const proto::ext::ui::UpdateToastRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  res->set_allocated_update_toast(ack);

  return res;
}

proto::ext::ui::Response *
UIRequestRouter::handleSetSearchText(const proto::ext::ui::SetSearchTextRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  m_navigation->handle()->setSearchText(req.text().c_str());
  res->set_allocated_set_search_text(ack);

  return res;
}

proto::ext::ui::Response *
UIRequestRouter::handleCloseWindow(const proto::ext::ui::CloseMainWindowRequest &req) {
  using namespace std::chrono_literals;
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;
  auto popToRoot = parseProtoPopToRoot(req.pop_to_root());
  auto clear = req.clear_root_search();

  // FIXME: this slight delay was introduced so that we can process a copy or paste that directly follows
  // a call to closeWindow. Some compositors such as niri just don't register the copy at all if it's done
  // after the window is closed.
  // This is very much a hack and we should figure out a way to fix this.
  m_navigation->handle()->closeWindow({.popToRootType = popToRoot, .clearRootSearch = clear}, 50ms);
  res->set_allocated_close_main_window(ack);

  return res;
}

proto::ext::ui::Response *
UIRequestRouter::getSelectedText(const proto::ext::ui::GetSelectedTextRequest &req) {
  auto text = QApplication::clipboard()->text(QClipboard::Mode::Selection);
  auto res = new proto::ext::ui::Response;
  auto selectedTextRes = new proto::ext::ui::GetSelectedTextResponse;

  selectedTextRes->set_text(text.toStdString());
  res->set_allocated_get_selected_text(selectedTextRes);

  return res;
}

class ExtensionAlert : public AlertWidget {
  void confirm() const override {}

  void canceled() const override {}
};

QFuture<proto::ext::extension::Response *> UIRequestRouter::confirmAlert(const ui::ConfirmAlertRequest &req) {
  auto alert = new CallbackAlertWidget;
  auto controller = m_navigation->controller();
  auto promise = std::make_shared<QPromise<proto::ext::extension::Response *>>();
  auto future = promise->future();

  alert->setTitle(req.title().c_str());
  alert->setMessage(req.description().c_str());
  alert->setConfirmText(req.primary_action().title().c_str(), SemanticColor::Red);

  if (req.has_icon()) {
    ImageURL url = ProtoUrl::parse(req.icon());
    if (url.isBuiltin()) { url.setFill(SemanticColor::Red); }
    alert->setIcon(url);
  }

  alert->setCancelText(req.dismiss_action().title().c_str(), SemanticColor::Foreground);

  alert->setCallback([promise](bool value) mutable {
    auto res = new proto::ext::ui::Response;
    auto ack = new proto::ext::ui::ConfirmAlertResponse;

    ack->set_confirmed(value);
    res->set_allocated_confirm_alert(ack);
    promise->addResult(wrapUI(res));
    promise->finish();
  });

  m_navigation->handle()->setDialog(alert);

  return future;
}

proto::ext::ui::Response *UIRequestRouter::popToRoot(const proto::ext::ui::PopToRootRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  m_navigation->handle()->popToRoot({.clearSearch = req.clear_search_bar()});
  res->set_allocated_pop_to_root(ack);

  return res;
}

proto::ext::ui::Response *UIRequestRouter::pushView(const proto::ext::ui::PushViewRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  m_navigation->pushView();
  res->set_allocated_close_main_window(ack);

  return res;
}

proto::ext::ui::Response *UIRequestRouter::popView(const proto::ext::ui::PopViewRequest &req) {
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  m_navigation->popView();
  res->set_allocated_pop_view(ack);

  return res;
}

proto::ext::ui::Response *UIRequestRouter::handleRender(const proto::ext::ui::RenderRequest &request) {
  /**
   * For now, we still process the render tree as JSON. Maybe later we can move that to protobuf as well,
   * but that will require writing more serialization code in the reconciler.
   */
  QJsonParseError parseError;
  auto doc = QJsonDocument::fromJson(request.json().c_str(), &parseError);

  if (parseError.error) {
    qWarning() << "Failed to parse render tree";
    return {};
  }

  auto views = doc.object().value("views").toArray();

  if (m_modelWatcher.isRunning()) {
    m_modelWatcher.cancel();
    m_modelWatcher.waitForFinished();
  }

  m_modelWatcher.setFuture(QtConcurrent::run([views]() {
    Timer timer;
    auto model = ModelParser().parse(views);

    // timer.time("Model parsed");
    return model;
  }));

  auto response = new proto::ext::ui::Response;

  response->set_allocated_render(new proto::ext::common::AckResponse);

  // render queued
  return response;
}

proto::ext::extension::Response *UIRequestRouter::wrapUI(proto::ext::ui::Response *uiRes) {
  auto res = new proto::ext::extension::Response;
  auto data = new proto::ext::extension::ResponseData;

  data->set_allocated_ui(uiRes);
  res->set_allocated_data(data);
  return res;
};
