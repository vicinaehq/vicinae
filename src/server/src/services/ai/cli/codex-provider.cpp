#include "services/ai/history-compaction.hpp"
#include "codex-provider.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <ranges>
#include <glaze/json.hpp>
#include "cli-process.hpp"
#include "generated/version.h"
#include "services/ai/ai-provider-types.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "vicinae.hpp"

namespace AI {

namespace codex {

struct RpcError {
  std::optional<std::string> message;
};

struct Incoming {
  std::optional<glz::raw_json> id;
  std::optional<std::string> method;
  std::optional<glz::raw_json> params;
  std::optional<glz::raw_json> result;
  std::optional<RpcError> error;
};

struct AccountResult {
  std::optional<glz::raw_json> account;
  bool requiresOpenaiAuth = false;
};

struct ModelInfo {
  std::string id;
  std::string displayName;
  std::string description;
  bool hidden = false;
  std::vector<std::string> inputModalities;
  bool isDefault = false;
};

struct ModelListParams {
  int limit = 100;
};

struct ModelListResult {
  std::vector<ModelInfo> data;
};

struct ThreadStartParams {
  bool ephemeral = true;
  std::string cwd;
  std::string sandbox = "read-only";
  std::string approvalPolicy = "never";
  std::string baseInstructions;
};

struct ThreadStartResult {
  struct Thread {
    std::string id;
  };
  Thread thread;
};

struct ThreadParams {
  std::string threadId;
};

struct ContentItem {
  std::string type;
  std::optional<std::string> text;
  std::optional<std::string> image_url;
};

struct ResponseItem {
  std::string type = "message";
  std::string role;
  std::vector<ContentItem> content;
};

struct InjectItemsParams {
  std::string threadId;
  std::vector<ResponseItem> items;
};

struct UserInput {
  std::string type;
  std::optional<std::string> text;
  std::optional<std::vector<std::string>> text_elements;
  std::optional<std::string> url;
};

struct TurnStartParams {
  std::string threadId;
  std::vector<UserInput> input;
  std::string model;
  std::string effort;
};

struct TurnStartResult {
  struct Turn {
    std::string id;
  };
  Turn turn;
};

struct TurnInterruptParams {
  std::string threadId;
  std::string turnId;
};

struct TurnError {
  std::string message;
};

struct ThreadRef {
  std::optional<std::string> threadId;
};

struct DeltaParams {
  std::string delta;
};

struct ErrorParams {
  TurnError error;
  bool willRetry = false;
};

struct TurnCompletedParams {
  struct Turn {
    std::string status;
    std::optional<TurnError> error;
  };
  Turn turn;
};

struct NestedError {
  struct Error {
    std::optional<std::string> message;
  };
  std::optional<Error> error;
};

} // namespace codex

namespace {

constexpr glz::opts LENIENT{.error_on_unknown_keys = false};
constexpr std::string_view DEFAULT_INSTRUCTIONS = "You are a helpful assistant.";

// Codex runs its tools itself, out of sight of the chat and without approval, so they are turned off.
// Unknown features are ignored in this form, which keeps older and newer versions working.
constexpr auto DISABLED_FEATURES = std::to_array<std::string_view>({
    "shell_tool",
    "unified_exec",
    "view_image",
    "image_generation",
    "apps",
    "plugins",
    "multi_agent",
    "computer_use",
    "browser_use",
    "in_app_browser",
    "sleep_tool",
    "tool_suggest",
    "skill_search",
    "hooks",
    "memories",
});

QStringList serverArguments() {
  QStringList args{"app-server", "-c", R"(web_search="disabled")", "-c", "mcp_servers={}"};
  args.reserve(args.size() + static_cast<qsizetype>(DISABLED_FEATURES.size()) * 2);
  for (const auto feature : DISABLED_FEATURES) {
    args << "-c"
         << QStringLiteral("features.%1=false")
                .arg(QString::fromUtf8(feature.data(), static_cast<qsizetype>(feature.size())));
  }
  return args;
}

template <typename T> std::string toJson(const T &value) {
  std::string out;
  if (glz::write_json(value, out)) return "{}";
  return out;
}

template <typename T> std::expected<T, std::string> fromJson(std::string_view json) {
  T value;
  if (const auto error = glz::read<LENIENT>(value, json))
    return std::unexpected(glz::format_error(error, json));
  return value;
}

// API errors arrive as the JSON body of the failed request.
std::string readableError(std::string message) {
  if (auto nested = fromJson<codex::NestedError>(message); nested && nested->error && nested->error->message)
    return *nested->error->message;
  return message;
}

std::string effortFor(ThinkingMode mode) {
  switch (mode) {
  case ThinkingMode::None:
  case ThinkingMode::Low:
    return "low";
  case ThinkingMode::Medium:
    return "medium";
  case ThinkingMode::High:
    return "high";
  }
  return "medium";
}

std::string dataUrl(const ImagePart &image) {
  return std::format("data:{};base64,{}", image.mimeType, image.base64.toStdString());
}

std::optional<std::string> partText(const ChatPart &part) {
  if (const auto *text = std::get_if<TextPart>(&part)) return text->text;
  if (const auto *call = std::get_if<ToolCallPart>(&part))
    return std::format("(called tool {} with {})", call->name, call->arguments);
  if (const auto *result = std::get_if<ToolResultPart>(&part))
    return std::format("(tool result: {})", result->content);
  return std::nullopt;
}

codex::ResponseItem toResponseItem(const ChatMessage &message) {
  const bool assistant = message.role == ChatRole::Assistant;
  codex::ResponseItem item{.role = assistant ? "assistant" : "user"};
  item.content.reserve(message.parts.size());

  for (const auto &part : message.parts) {
    if (auto text = partText(part)) {
      item.content.emplace_back(
          codex::ContentItem{.type = assistant ? "output_text" : "input_text", .text = std::move(*text)});
    } else if (const auto *image = std::get_if<ImagePart>(&part); image && !assistant) {
      item.content.emplace_back(codex::ContentItem{.type = "input_image", .image_url = dataUrl(*image)});
    }
  }

  return item;
}

std::vector<codex::UserInput> toUserInput(const ChatMessage &message) {
  std::vector<codex::UserInput> input;
  input.reserve(message.parts.size());

  for (const auto &part : message.parts) {
    if (auto text = partText(part)) {
      input.emplace_back(codex::UserInput{
          .type = "text", .text = std::move(*text), .text_elements = std::vector<std::string>{}});
    } else if (const auto *image = std::get_if<ImagePart>(&part)) {
      input.emplace_back(codex::UserInput{.type = "image", .url = dataUrl(*image)});
    }
  }

  return input;
}

} // namespace

class CodexChatStream : public AbstractChatCompletionStream {
public:
  CodexChatStream(CodexProvider &provider, codex::ThreadStartParams thread,
                  std::vector<codex::ResponseItem> history, codex::TurnStartParams turn, Model model)
      : m_provider(&provider), m_thread(std::move(thread)), m_history(std::move(history)),
        m_turn(std::move(turn)) {
    setModel(std::move(model));
  }

  ~CodexChatStream() override { abort(); }

  bool start() override {
    if (!m_provider) return false;
    m_active = true;

    m_provider->request("thread/start", toJson(m_thread),
                        [self = QPointer(this)](CodexProvider::Response response) {
                          if (self) self->handleThreadStarted(std::move(response));
                        });
    return true;
  }

  bool abort() override {
    if (!std::exchange(m_active, false)) return true;
    if (m_provider && m_turnId)
      m_provider->request("turn/interrupt", toJson(codex::TurnInterruptParams{.threadId = m_turn.threadId,
                                                                              .turnId = *m_turnId}));
    release();
    return true;
  }

  void handleNotification(std::string_view method, std::string_view params) {
    if (!m_active) return;

    if (method == "item/agentMessage/delta") {
      if (auto delta = fromJson<codex::DeltaParams>(params); delta && !delta->delta.empty())
        emit dataAdded(delta->delta);
    } else if (method.starts_with("item/reasoning/")) {
      emit activityChanged(ResponseActivity::Thinking);
    } else if (method == "error") {
      if (auto error = fromJson<codex::ErrorParams>(params); error && !error->willRetry)
        m_error = readableError(std::move(error->error.message));
    } else if (method == "turn/completed") {
      auto completed = fromJson<codex::TurnCompletedParams>(params);
      if (!completed) return fail(completed.error());

      const auto &turn = completed->turn;
      if (turn.status == "completed") {
        m_active = false;
        release();
        emit finished();
      } else if (turn.error) {
        fail(readableError(turn.error->message));
      } else {
        fail(m_error.value_or(std::format("Codex ended the response ({}).", turn.status)));
      }
    }
  }

  void handleServerStopped(const std::string &reason) {
    m_provider = nullptr;
    fail(std::format("Codex stopped: {}", reason));
  }

private:
  void handleThreadStarted(CodexProvider::Response response) {
    if (!m_active) return;
    if (!response) return fail(response.error());

    auto result = fromJson<codex::ThreadStartResult>(*response);
    if (!result) return fail(result.error());

    m_turn.threadId = std::move(result->thread.id);
    m_provider->watchThread(m_turn.threadId, this);

    if (m_history.empty()) return startTurn();

    m_provider->request(
        "thread/inject_items",
        toJson(codex::InjectItemsParams{.threadId = m_turn.threadId, .items = std::move(m_history)}),
        [self = QPointer(this)](CodexProvider::Response response) {
          if (!self || !self->m_active) return;
          if (!response) return self->fail(response.error());
          self->startTurn();
        });
  }

  void startTurn() {
    m_provider->request("turn/start", toJson(m_turn),
                        [self = QPointer(this)](CodexProvider::Response response) {
                          if (!self || !self->m_active) return;
                          if (!response) return self->fail(readableError(std::move(response.error())));
                          if (auto result = fromJson<codex::TurnStartResult>(*response))
                            self->m_turnId = std::move(result->turn.id);
                        });
  }

  void fail(std::string reason) {
    if (!std::exchange(m_active, false)) return;
    release();
    emit errorOccurred(reason);
  }

  void release() {
    if (!m_provider || m_turn.threadId.empty()) return;
    m_provider->unwatchThread(m_turn.threadId);
    m_provider->request("thread/unsubscribe", toJson(codex::ThreadParams{.threadId = m_turn.threadId}));
  }

  QPointer<CodexProvider> m_provider;
  codex::ThreadStartParams m_thread;
  std::vector<codex::ResponseItem> m_history;
  codex::TurnStartParams m_turn;
  std::optional<std::string> m_turnId;
  std::optional<std::string> m_error;
  bool m_active = false;
};

CodexProvider::CodexProvider(std::string id) : m_id(std::move(id)) {}

CodexProvider::~CodexProvider() {
  if (m_server) m_server->shutdown();
}

std::string CodexProvider::displayName() const { return std::string(findProviderType(TYPE)->label); }

std::optional<ImageUrl> CodexProvider::icon() const { return ImageUrl{BuiltinIcon::Codex}; }

std::string_view CodexProvider::description() const { return findProviderType(TYPE)->description; }

void CodexProvider::configure(const PreferenceValues &values) {
  auto fields = readPreferences<CodexFields>(values);
  if (fields.executable == m_executableSetting && m_started) return;

  m_executableSetting = std::move(fields.executable);
  if (!m_started) return;

  stop("The configuration changed.");
  launch();
}

void CodexProvider::start() {
  m_started = true;
  launch();
}

void CodexProvider::launch() {
  if (m_server) return;

  m_executable = findCliExecutable(m_executableSetting, QStringLiteral("codex"));
  if (!m_executable) {
    m_status =
        m_executableSetting.empty()
            ? tr("Codex is not installed, or could not be found.").toStdString()
            : tr("No executable at %1.").arg(QString::fromStdString(m_executableSetting)).toStdString();
    emit modelsUpdated();
    return;
  }

  if (m_models.empty()) {
    m_status = tr("Checking Codex…").toStdString();
    emit modelsUpdated();
  }

  m_server = new JsonLineProcess(*m_executable, serverArguments(), this);
  connect(m_server, &JsonLineProcess::lineReceived, this, &CodexProvider::handleLine);
  connect(m_server, &JsonLineProcess::exited, this, [this](const std::string &reason) {
    qWarning() << "Codex app server exited:" << reason;
    stop(reason);
    m_status = tr("Codex stopped unexpectedly.").toStdString();
    emit modelsUpdated();
  });
  m_server->start();

  const auto id = m_nextId++;
  m_pending[id] = [this](Response response) {
    if (!response) {
      qWarning() << "Failed to initialize the Codex app server:" << response.error();
      return;
    }
    handleInitialized();
  };
  m_server->write(std::format(
      R"({{"id":{},"method":"initialize","params":{{"clientInfo":{{"name":"vicinae","title":"Vicinae","version":"{}"}},"capabilities":null}}}})",
      id, VICINAE_GIT_TAG));
}

void CodexProvider::stop(const std::string &reason) {
  m_ready = false;
  m_queue.clear();

  for (auto &[id, handler] : std::exchange(m_pending, {})) {
    if (handler) handler(std::unexpected(std::format("Codex stopped: {}", reason)));
  }

  for (auto &[id, stream] : std::exchange(m_threads, {})) {
    if (stream) stream->handleServerStopped(reason);
  }

  if (auto *server = std::exchange(m_server, nullptr)) {
    disconnect(server, nullptr, this, nullptr);
    server->shutdown();
  }
}

void CodexProvider::handleInitialized() {
  m_server->write(R"({"method":"initialized"})");
  m_ready = true;
  for (auto &line : std::exchange(m_queue, {}))
    m_server->write(line);

  request("account/read", "{}", [this](Response response) { handleAccount(std::move(response)); });
}

void CodexProvider::handleAccount(Response response) {
  if (!response) return;

  auto account = fromJson<codex::AccountResult>(*response);
  m_signedIn = account && (account->account || !account->requiresOpenaiAuth);

  if (!m_signedIn) {
    m_status = tr("Not signed in. Run \"codex login\" in a terminal.").toStdString();
    emit modelsUpdated();
    return;
  }

  request("model/list", toJson(codex::ModelListParams{}),
          [this](Response response) { handleModels(std::move(response)); });
}

void CodexProvider::handleModels(Response response) {
  auto result =
      response.and_then([](const std::string &json) { return fromJson<codex::ModelListResult>(json); });
  if (!result) {
    qWarning() << "Failed to list Codex models:" << result.error();
    m_status = tr("Could not list Codex models.").toStdString();
    emit modelsUpdated();
    return;
  }

  std::ranges::stable_partition(result->data, &codex::ModelInfo::isDefault);
  m_models.clear();
  m_models.reserve(result->data.size());

  for (auto &info : result->data) {
    if (info.hidden) continue;
    Capabilities caps = Capability::Completion | Capability::Thinking;
    if (std::ranges::contains(info.inputModalities, "image")) caps |= Capability::Vision;
    m_models.emplace_back(Model{.id = std::move(info.id),
                                .name = std::move(info.displayName),
                                .description = std::move(info.description),
                                .icon = ImageUrl{BuiltinIcon::Openai},
                                .caps = caps});
  }

  m_status.reset();
  emit modelsUpdated();
}

void CodexProvider::request(std::string_view method, std::string params, ResponseHandler handler) {
  if (!m_server) {
    if (handler) handler(std::unexpected(m_status.value_or("Codex is not available.")));
    return;
  }

  const auto id = m_nextId++;
  if (handler) m_pending[id] = std::move(handler);
  auto line = std::format(R"({{"id":{},"method":"{}","params":{}}})", id, method, params);

  if (m_ready) {
    m_server->write(line);
  } else {
    m_queue.emplace_back(std::move(line));
  }
}

void CodexProvider::watchThread(const std::string &threadId, CodexChatStream *stream) {
  m_threads[threadId] = stream;
}

void CodexProvider::unwatchThread(const std::string &threadId) { m_threads.erase(threadId); }

void CodexProvider::handleLine(const std::string &line) {
  auto message = fromJson<codex::Incoming>(line);
  if (!message) {
    qWarning() << "Failed to parse Codex message:" << message.error();
    return;
  }

  // Requests from the server, such as approvals, which are never needed with the read-only sandbox.
  if (message->method && message->id) {
    m_server->write(std::format(
        R"({{"id":{},"error":{{"code":-32601,"message":"Not supported by Vicinae"}}}})", message->id->str));
    return;
  }

  if (message->method) {
    const std::string_view params = message->params ? std::string_view(message->params->str) : "{}";
    auto ref = fromJson<codex::ThreadRef>(params);
    if (!ref || !ref->threadId) return;
    if (auto it = m_threads.find(*ref->threadId); it != m_threads.end() && it->second)
      it->second->handleNotification(*message->method, params);
    return;
  }

  if (!message->id) return;

  std::int64_t id = 0;
  const auto &raw = message->id->str;
  if (std::from_chars(raw.data(), raw.data() + raw.size(), id).ec != std::errc{}) return;

  auto node = m_pending.extract(id);
  if (node.empty() || !node.mapped()) return;

  if (message->error) {
    node.mapped()(std::unexpected(message->error->message.value_or("Unknown error")));
  } else {
    node.mapped()(message->result ? message->result->str : std::string("{}"));
  }
}

ModelList CodexProvider::listModels(const ListModelFilters &filters) const {
  return m_models | std::views::filter([&](const Model &model) {
           return !filters.caps || (model.caps & *filters.caps);
         }) |
         std::ranges::to<ModelList>();
}

std::optional<Model> CodexProvider::findBestModel(Capabilities caps, Preference preference) const {
  if (preference != Preference::None) return std::nullopt;
  const auto models = listModels({.caps = caps});
  if (models.empty()) return std::nullopt;
  return models.front();
}

std::shared_ptr<AbstractChatCompletionStream>
CodexProvider::createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) {
  if (!m_signedIn) return nullptr;
  launch();

  const auto model = std::ranges::find(m_models, modelId, &Model::id);
  if (model == m_models.end()) return nullptr;

  codex::ThreadStartParams thread{.cwd = cliWorkingDirectory().toStdString()};
  std::vector<codex::ResponseItem> history;
  const auto messages = compactHistory(payload.messages);
  history.reserve(messages.size());
  const ChatMessage *last = nullptr;

  for (const auto &message : messages) {
    if (message.role == ChatRole::System || message.role == ChatRole::Developer) {
      if (!thread.baseInstructions.empty()) thread.baseInstructions += "\n\n";
      thread.baseInstructions += message.text();
      continue;
    }
    if (last) history.emplace_back(toResponseItem(*last));
    last = &message;
  }

  if (!last || last->role != ChatRole::User) return nullptr;
  if (thread.baseInstructions.empty()) thread.baseInstructions = DEFAULT_INSTRUCTIONS;

  std::erase_if(history, [](const codex::ResponseItem &item) { return item.content.empty(); });

  return std::make_shared<CodexChatStream>(*this, std::move(thread), std::move(history),
                                           codex::TurnStartParams{.input = toUserInput(*last),
                                                                  .model = model->id,
                                                                  .effort = effortFor(payload.thinking)},
                                           *model);
}

QFuture<TranscriptionResult> CodexProvider::transcribe(Audio::Recording, const TranscriptionOptions &) {
  return QtFuture::makeReadyValueFuture<TranscriptionResult>(
      std::unexpected("Codex does not support transcription"));
}

} // namespace AI
