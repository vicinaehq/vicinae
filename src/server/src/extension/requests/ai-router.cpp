#include "ai-router.hpp"
#include "extension/extension-command-controller.hpp"
#include "proto/ai.pb.h"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <stdexcept>

namespace ext = proto::ext::extension;

AIRouter::AIRouter(AI::Service &ai, ExtensionCommandController &commandController)
    : m_app(ai), m_commandController(commandController) {}

PromiseLike<ext::Response *> AIRouter::route(const proto::ext::ai::Request &req) {
  switch (req.payload_case()) {
  case proto::ext::ai::Request::kAsk:
    return wrapResponse(ask(req.ask()));
  case proto::ext::ai::Request::kAbortAsk:
    return wrapResponse(abortAsk(req.abort_ask()));
  case proto::ext::ai::Request::kGetModels:
    return wrapResponse(getModels(req.get_models()));
  default:
    break;
  }

  return nullptr;
}

proto::ext::ai::Response *AIRouter::ask(const proto::ext::ai::AskRequest &req) {
  auto res = new proto::ext::ai::Response;
  auto ask = new proto::ext::ai::AskResponse;
  AI::ChatCompletionPayload payload;
  std::optional<AI::ModelRef> model;

  payload.messages = {AI::ChatMessage(AI::ChatRole::User, req.prompt())};
  payload.temperature = req.temperature();

  if (req.has_model()) {
    auto const ref = AI::ModelRef::fromString(req.model());
    if (!ref) { throw std::runtime_error("Ill formed model identifier"); }
    model = std::move(ref).value();
  }

  auto completion = m_app.createChatCompletion(model, payload);

  connect(completion.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
          [this, id = req.handler()](const std::string &data) {
            QJsonArray args;
            args << data.c_str() << false;
            m_commandController.notify(id.c_str(), QJsonArray{data.c_str(), false});
          });
  connect(completion.get(), &AI::AbstractChatCompletionStream::finished, this, [this, id = req.handler()]() {
    QJsonArray args;
    m_commandController.notify(id.c_str(), QJsonArray{"", true});
    m_completions.erase(id);
  });

  completion->start();
  m_completions.insert({req.handler(), completion});
  res->set_allocated_ask(ask);
  return res;
}

proto::ext::ai::Response *AIRouter::abortAsk(const proto::ext::ai::AbortAskRequest &req) {
  auto res = new proto::ext::ai::Response;
  auto ask = new proto::ext::ai::AbortAskResponse;

  m_completions.erase(req.handler());
  res->set_allocated_abort_ask(ask);

  return res;
}

proto::ext::ai::Response *AIRouter::getModels(const proto::ext::ai::GetModelsRequest &req) {
  auto res = new proto::ext::ai::Response;
  auto getModelsRes = new proto::ext::ai::GetModelsResponse;

  res->set_allocated_getmodels(getModelsRes);

  for (const auto &model : m_app.listModels()) {
    if (!(model.caps & AI::Capability::Completion)) continue;
    auto pmodel = getModelsRes->add_models();

    pmodel->set_id(model.ref.toString());
    pmodel->set_name(model.name);

    for (const auto &cap : AI::stringifyCapabilities(model.caps)) {
      *pmodel->add_capabilities() = cap;
    }
  }

  return res;
}

proto::ext::extension::Response *AIRouter::wrapResponse(proto::ext::ai::Response *ai) {
  auto res = new proto::ext::extension::Response;
  auto data = new proto::ext::extension::ResponseData;

  data->set_allocated_ai(ai);
  res->set_allocated_data(data);
  return res;
}
