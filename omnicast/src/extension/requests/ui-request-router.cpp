#include "ui-request-router.hpp"

void UIRequestRouter::modelCreated() {
  if (m_modelWatcher.isCanceled()) return;

  auto views = m_navigation->views();
  auto models = m_modelWatcher.result();
  auto items = models.items | std::views::take(views.size()) | std::views::enumerate;

  for (const auto &[n, model] : items) {
    auto view = views.at(n);
    bool shouldSkipRender = !model.dirty && !model.propsDirty;

    if (shouldSkipRender) {
      qDebug() << "view" << n << "is not dirty, skipping render";
      continue;
    }

    view->render(model.root);
  }
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
  auto res = new proto::ext::ui::Response;
  auto ack = new proto::ext::common::AckResponse;

  m_navigation->handle()->closeWindow();
  res->set_allocated_close_main_window(ack);

  return res;
}

proto::ext::extension::Response *UIRequestRouter::route(const proto::ext::ui::Request &req) {
  using Request = proto::ext::ui::Request;

  auto wrapUI = [](proto::ext::ui::Response *uiRes) -> proto::ext::extension::Response * {
    auto res = new proto::ext::extension::Response;
    auto data = new proto::ext::extension::ResponseData;

    data->set_allocated_ui(uiRes);
    res->set_allocated_data(data);
    return res;
  };

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
    return wrapUI(pushView(req.push_view()));
  default:
    break;
  }

  return nullptr;
  // return makeErrorResponse("Unhandled UI request");
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
    qCritical() << "Failed to parse render tree";
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

    timer.time("Model parsed");
    return model;
  }));

  auto response = new proto::ext::ui::Response;

  response->set_allocated_render(new proto::ext::common::AckResponse);

  // render queued
  return response;
}
