#pragma once
#include "extension/extension-command-controller.hpp"
#include "proto/ai.pb.h"
#include "proto/extension.pb.h"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include "types.hpp"
#include <absl/strings/str_format.h>
#include <qobject.h>

class AIRouter : public QObject {
public:
  PromiseLike<proto::ext::extension::Response *> route(const proto::ext::ai::Request &req);
  AIRouter(AI::Service &ai, ExtensionCommandController &controller);

private:
  struct AskCompletion {
    std::string id;
    std::shared_ptr<AI::AbstractChatCompletionStream> completion;
  };

  proto::ext::ai::Response *ask(const proto::ext::ai::AskRequest &req);
  proto::ext::ai::Response *abortAsk(const proto::ext::ai::AbortAskRequest &req);
  proto::ext::ai::Response *getModels(const proto::ext::ai::GetModelsRequest &req);

  static proto::ext::extension::Response *wrapResponse(proto::ext::ai::Response *res);

  AI::Service &m_app;
  ExtensionCommandController &m_commandController;
  std::unordered_map<std::string, std::shared_ptr<AI::AbstractChatCompletionStream>> m_completions;
};
