#include "acp-provider.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <ranges>
#include <span>
#include <glaze/json.hpp>
#include <QCryptographicHash>
#include <QProcess>
#include "generated/version.h"
#include "services/ai/ai-provider-types.hpp"
#include "services/ai/cli/cli-process.hpp"
#include "services/ai/history-compaction.hpp"

namespace AI {

// not anonymous: glaze can't reflect types with internal linkage
namespace acp {

struct Incoming {
  std::optional<glz::raw_json> id;
  std::optional<std::string> method;
  std::optional<glz::raw_json> params;
  std::optional<glz::raw_json> result;
  std::optional<AcpProvider::RpcError> error;
};

struct PromptCapabilities {
  bool image = false;
};

struct SessionCapabilities {
  std::optional<glz::raw_json> close;
};

struct AgentCapabilities {
  std::optional<PromptCapabilities> promptCapabilities;
  std::optional<SessionCapabilities> sessionCapabilities;
};

struct InitializeResult {
  std::optional<AgentCapabilities> agentCapabilities;
};

struct ConfigChoice {
  std::string value;
  std::optional<std::string> name;
  std::optional<std::string> description;
};

// a choice, or a group of them
struct ConfigEntry {
  std::optional<std::string> value;
  std::optional<std::string> name;
  std::optional<std::string> description;
  std::optional<std::vector<ConfigChoice>> options;
};

struct ConfigOption {
  std::string id;
  std::optional<std::string> category;
  std::optional<std::string> type;
  std::optional<glz::raw_json> currentValue;
  std::optional<std::vector<ConfigEntry>> options;
};

struct NewSessionResult {
  std::string sessionId;
  std::optional<glz::raw_json> configOptions;
};

struct ContentBlock {
  std::optional<std::string> type;
  std::optional<std::string> text;
};

struct ToolContent {
  std::optional<std::string> type;
  std::optional<ContentBlock> content;
  std::optional<std::string> path;
  std::optional<std::string> newText;
};

// `content` is a content block for messages and a list of tool contents for tool calls
struct Update {
  std::optional<std::string> sessionUpdate;
  std::optional<glz::raw_json> content;
  std::optional<std::string> toolCallId;
  std::optional<std::string> title;
  std::optional<std::string> kind;
  std::optional<std::string> status;
  std::optional<glz::raw_json> rawInput;
  std::optional<glz::raw_json> rawOutput;
};

struct SessionNotification {
  std::string sessionId;
  Update update;
};

struct PermissionOption {
  std::string optionId;
  std::string kind;
};

struct PermissionRequest {
  std::string sessionId;
  Update toolCall;
  std::vector<PermissionOption> options;
};

struct PromptResult {
  std::string stopReason;
};

struct PromptBlock {
  std::string type;
  std::optional<std::string> text;
  std::optional<std::string> data;
  std::optional<std::string> mimeType;
};

struct NewSessionParams {
  std::string cwd;
  std::vector<std::string> mcpServers;
};

struct PromptParams {
  std::string sessionId;
  std::vector<PromptBlock> prompt;
};

struct SetConfigOptionParams {
  std::string sessionId;
  std::string configId;
  std::string value;
};

struct SessionRef {
  std::string sessionId;
};

} // namespace acp

namespace {

constexpr glz::opts LENIENT{.error_on_unknown_keys = false};
constexpr std::size_t MAX_TOOL_OUTPUT = std::size_t{16} * 1024;
constexpr std::string_view CANCELLED = R"({"outcome":{"outcome":"cancelled"}})";
constexpr std::string_view DEFAULT_MODEL = "default";
constexpr auto FAST_MARKERS = std::to_array<std::string_view>({"haiku", "mini", "flash", "fast"});

struct AgentPreset {
  std::string_view type;
  std::string_view executable;
  std::string_view arguments;
  // run through npx when the executable isn't installed, pinned to a version we know works
  std::string_view package;
  std::string_view login;
  // would make the agent bill it instead of using the user's sign-in
  std::string_view apiKeyVariable;
};

constexpr auto PRESETS = std::to_array<AgentPreset>({
    {"claude-code", "claude-agent-acp", "", "@agentclientprotocol/claude-agent-acp@0.101.0", "claude",
     "ANTHROPIC_API_KEY"},
    {"codex", "codex-acp", "", "@agentclientprotocol/codex-acp@2.6.0", "codex login"},
    {"gemini", "gemini", "--acp", "", "gemini"},
    {"opencode", "opencode", "acp", "", "opencode auth login"},
    {"qwen-code", "qwen", "--acp", "", "qwen"},
    {"kilo", "kilo", "acp", "", "kilo auth login"},
});

const AgentPreset *findPreset(std::string_view type) {
  const auto it = std::ranges::find(PRESETS, type, &AgentPreset::type);
  return it != PRESETS.end() ? &*it : nullptr;
}

QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
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

bool present(const std::optional<glz::raw_json> &json) { return json && json->str != "null"; }

std::string truncated(std::string_view text) { return std::string(truncateUtf8(text, MAX_TOOL_OUTPUT).text); }

/**
 * Hash of a user message and the text of the reply. Agents keep their own record of tool calls, so a
 * conversation is identified by what was said only.
 */
QByteArray chainExchange(QByteArrayView previous, const ChatMessage &user, std::string_view reply) {
  QCryptographicHash hash(QCryptographicHash::Sha256);
  const auto add = [&](char tag, std::string_view data) {
    hash.addData(QByteArrayView(&tag, 1));
    hash.addData(QByteArrayView(data.data(), static_cast<qsizetype>(data.size())));
    hash.addData(QByteArrayView("\0", 1));
  };

  hash.addData(previous);
  for (const auto &part : user.parts) {
    if (const auto *text = std::get_if<TextPart>(&part)) {
      add('t', text->text);
    } else if (const auto *image = std::get_if<ImagePart>(&part)) {
      add('i', image->mimeType);
      hash.addData(image->base64);
    }
  }
  add('r', reply);
  return hash.result();
}

QByteArray historyFingerprint(std::span<const ChatMessage> history) {
  QByteArray fingerprint;
  const ChatMessage *user = nullptr;
  std::string reply;

  for (const auto &message : history) {
    if (message.role == ChatRole::User) {
      if (user) fingerprint = chainExchange(fingerprint, *user, reply);
      user = &message;
      reply.clear();
    } else if (message.role == ChatRole::Assistant) {
      reply += message.text();
    }
  }
  if (user) fingerprint = chainExchange(fingerprint, *user, reply);
  return fingerprint;
}

void appendBlocks(std::vector<acp::PromptBlock> &blocks, const ChatMessage &message, bool images) {
  const auto text = [&](std::string value) {
    blocks.emplace_back(acp::PromptBlock{.type = "text", .text = std::move(value)});
  };

  for (const auto &part : message.parts) {
    if (const auto *value = std::get_if<TextPart>(&part)) {
      text(value->text);
    } else if (const auto *image = std::get_if<ImagePart>(&part)) {
      if (images) {
        blocks.emplace_back(acp::PromptBlock{
            .type = "image",
            .data = image->base64.toStdString(),
            .mimeType = image->mimeType,
        });
      } else {
        text("(image not shown)");
      }
    } else if (const auto *call = std::get_if<ToolCallPart>(&part)) {
      text(std::format("(used tool {} with {})", call->name, call->arguments));
    } else if (const auto *result = std::get_if<ToolResultPart>(&part)) {
      text(std::format("(tool result: {})", result->content));
    }
  }
}

/**
 * Agents can't be handed earlier turns, so a conversation they haven't seen is replayed as a transcript
 * inside the first message.
 */
std::vector<acp::PromptBlock> makePrompt(std::string_view instructions, const ChatHistory &history,
                                         const ChatMessage &message, bool images) {
  std::vector<acp::PromptBlock> blocks;
  blocks.reserve(history.size() * 3 + message.parts.size() + 3);
  const auto text = [&](std::string value) {
    blocks.emplace_back(acp::PromptBlock{.type = "text", .text = std::move(value)});
  };

  if (!instructions.empty()) text(std::format("<instructions>\n{}\n</instructions>", instructions));

  if (!history.empty()) {
    text("This conversation started elsewhere. Here is what was said so far:\n\n<conversation>");
    for (const auto &entry : history) {
      const auto tag = roleName(entry.role);
      text(std::format("<{}>", tag));
      appendBlocks(blocks, entry, images);
      text(std::format("</{}>", tag));
    }
    text("</conversation>\n\nContinue it by replying to this message:");
  }

  appendBlocks(blocks, message, images);
  return blocks;
}

std::optional<std::string> toolOutput(std::string_view json) {
  auto contents = fromJson<std::vector<acp::ToolContent>>(json);
  if (!contents) return std::nullopt;

  std::string out;
  for (const auto &content : *contents) {
    std::string piece;
    if (content.type == "content" && content.content && content.content->text) {
      piece = *content.content->text;
    } else if (content.type == "diff" && content.path) {
      piece = std::format("{}\n{}", *content.path, content.newText.value_or(""));
    }
    if (piece.empty()) continue;
    if (!out.empty()) out += '\n';
    out += piece;
  }
  if (out.empty()) return std::nullopt;
  return truncated(out);
}

} // namespace

class AcpChatStream : public AbstractChatCompletionStream {
public:
  AcpChatStream(AcpProvider *provider, std::optional<std::string> sessionId, std::string modelId,
                std::vector<acp::PromptBlock> prompt, QByteArray history, ChatMessage message, Model model,
                bool oneShot)
      : m_provider(provider), m_sessionId(std::move(sessionId)), m_modelId(std::move(modelId)),
        m_prompt(std::move(prompt)), m_history(std::move(history)), m_message(std::move(message)),
        m_oneShot(oneShot) {
    setModel(std::move(model));
  }

  // NOLINTBEGIN(bugprone-exception-escape)
  ~AcpChatStream() override { abort(); }
  // NOLINTEND(bugprone-exception-escape)

  bool start() override {
    if (!m_provider || m_active) return false;
    m_active = true;
    m_provider->runTurn(this);
    return true;
  }

  bool abort() override {
    if (!std::exchange(m_active, false)) return true;
    if (!m_provider) return true;
    for (const auto &permission : std::exchange(m_permissions, {}))
      m_provider->respond(permission.requestId, CANCELLED);
    if (m_sessionId) m_provider->cancelTurn(*m_sessionId);
    return true;
  }

  void resolveApproval(std::string_view callId, bool approved, bool always) override {
    const auto permission = std::ranges::find(m_permissions, callId, &Permission::callId);
    if (permission == m_permissions.end() || !m_provider) return;

    const auto pick = [&](std::initializer_list<std::string_view> kinds) -> std::optional<std::string> {
      for (const auto kind : kinds) {
        const auto option = std::ranges::find(permission->options, kind, &acp::PermissionOption::kind);
        if (option != permission->options.end()) return option->optionId;
      }
      return std::nullopt;
    };
    const auto option = !approved ? pick({"reject_once", "reject_always"})
                        : always  ? pick({"allow_always", "allow_once"})
                                  : pick({"allow_once", "allow_always"});

    m_provider->respond(
        permission->requestId,
        option ? std::format(R"({{"outcome":{{"outcome":"selected","optionId":{}}}}})", toJson(*option))
               : std::string(CANCELLED));
    m_permissions.erase(permission);
    if (!m_permissions.empty()) emit approvalRequested(m_permissions.front().callId);
  }

  bool active() const { return m_active; }
  bool oneShot() const { return m_oneShot; }
  const std::optional<std::string> &sessionId() const { return m_sessionId; }
  void setSessionId(std::string id) { m_sessionId = std::move(id); }
  const std::string &modelId() const { return m_modelId; }
  std::string promptJson() const { return toJson(acp::PromptParams{*m_sessionId, m_prompt}); }
  QByteArray fingerprint() const { return chainExchange(m_history, m_message, m_text); }

  void handleUpdate(const acp::Update &update) {
    if (!m_active) return;
    const auto kind = update.sessionUpdate.value_or("");

    if (kind == "agent_message_chunk") {
      if (!present(update.content)) return;
      auto block = fromJson<acp::ContentBlock>(update.content->str);
      if (!block || block->type != "text" || !block->text || block->text->empty()) return;
      m_text += *block->text;
      emit dataAdded(*block->text);
    } else if (kind == "agent_thought_chunk") {
      emit activityChanged(ResponseActivity::Thinking);
    } else if (kind == "tool_call" || kind == "tool_call_update") {
      reportTool(update);
    }
  }

  void handlePermission(std::string requestId, acp::PermissionRequest request) {
    if (!m_active) {
      if (m_provider) m_provider->respond(requestId, CANCELLED);
      return;
    }

    auto callId = request.toolCall.toolCallId.value_or(std::format("permission-{}", requestId));
    request.toolCall.toolCallId = callId;
    reportTool(request.toolCall);
    if (!m_active) return;

    m_permissions.emplace_back(
        Permission{std::move(requestId), std::move(callId), std::move(request.options)});
    if (m_permissions.size() == 1) emit approvalRequested(m_permissions.front().callId);
  }

  /**
   * Nothing may touch this object afterwards: whoever owns it may let go of it as soon as it hears.
   */
  void complete(std::optional<std::string> error) {
    if (!std::exchange(m_active, false)) return;
    m_permissions.clear();
    if (error) {
      emit errorOccurred(*error);
    } else {
      emit finished();
    }
  }

private:
  struct Permission {
    std::string requestId;
    std::string callId;
    std::vector<acp::PermissionOption> options;
  };

  void reportTool(const acp::Update &update) {
    if (!update.toolCallId) return;

    auto tool = std::ranges::find_if(
        m_tools, [&](const ToolActivity &tool) { return tool.call.id == *update.toolCallId; });
    if (tool == m_tools.end()) {
      m_tools.emplace_back(ToolActivity{.call = {.id = *update.toolCallId, .name = "other"}});
      tool = m_tools.end() - 1;
    }

    if (update.kind) tool->call.name = *update.kind;
    if (update.title) tool->title = *update.title;
    if (present(update.rawInput)) tool->call.arguments = update.rawInput->str;
    if (update.status) {
      tool->status = *update.status == "completed" ? ToolActivity::Status::Succeeded
                     : *update.status == "failed"  ? ToolActivity::Status::Failed
                                                   : ToolActivity::Status::Running;
    }
    if (present(update.content)) {
      if (auto output = toolOutput(update.content->str)) tool->output = std::move(output);
    }
    if (!tool->output && present(update.rawOutput)) tool->output = truncated(update.rawOutput->str);

    emit toolActivityChanged(*tool);
  }

  QPointer<AcpProvider> m_provider;
  std::optional<std::string> m_sessionId;
  std::string m_modelId;
  std::vector<acp::PromptBlock> m_prompt;
  QByteArray m_history;
  ChatMessage m_message;
  std::string m_text;
  std::vector<ToolActivity> m_tools;
  std::vector<Permission> m_permissions;
  bool m_oneShot = false;
  bool m_active = false;
};

AcpProvider::AcpProvider(std::string id, std::string_view type) : m_id(std::move(id)), m_type(type) {
  m_idle.setSingleShot(true);
  m_idle.setInterval(IDLE_TIMEOUT);
  connect(&m_idle, &QTimer::timeout, this, [this] { stop("The agent was idle."); });
}

AcpProvider::~AcpProvider() {
  // running turns still get to fail, but nobody should hear about this provider anymore
  blockSignals(true);
  stop("The agent was removed.");
}

std::string AcpProvider::displayName() const {
  if (!m_name.empty()) return m_name;
  const auto *info = findProviderType(m_type);
  return info ? std::string(info->label) : m_id;
}

std::optional<ImageUrl> AcpProvider::icon() const {
  const auto *info = findProviderType(m_type);
  return info ? std::optional(ImageUrl{info->icon}) : std::nullopt;
}

std::string_view AcpProvider::description() const {
  const auto *info = findProviderType(m_type);
  return info ? info->description : std::string_view{};
}

std::optional<QString> AcpProvider::missingRequirement(std::string_view type) {
  const auto *preset = findPreset(type);
  if (!preset || findCliExecutable({}, qs(preset->executable))) return std::nullopt;
  if (preset->package.empty()) return tr("Not installed");
  if (findCliExecutable({}, QStringLiteral("npx"))) return std::nullopt;
  return tr("Requires Node.js");
}

std::string AcpProvider::signInMessage() const {
  if (const auto *preset = findPreset(m_type))
    return tr("Not signed in. Run \"%1\" in a terminal to sign in.").arg(qs(preset->login)).toStdString();
  return tr("Not signed in. Sign in to the agent from a terminal.").toStdString();
}

void AcpProvider::configure(const PreferenceValues &values) {
  std::string command;
  std::string name;
  if (findPreset(m_type)) {
    command = readPreferences<AgentFields>(values).command;
  } else {
    auto fields = readPreferences<CustomAgentFields>(values);
    command = std::move(fields.command);
    name = std::move(fields.name);
  }

  if (m_started && command == m_command && name == m_name) return;
  m_command = std::move(command);
  m_name = std::move(name);
  stop("The agent was reconfigured.");
  m_models.clear();
  m_modelOption.reset();
  m_defaultModel.reset();
  m_available = false;
  if (m_started) discover();
}

void AcpProvider::start() {
  m_started = true;
  discover();
}

void AcpProvider::discover() {
  m_status = tr("Starting %1…").arg(qs(displayName())).toStdString();
  emit modelsUpdated();

  connectAgent([this](std::expected<void, std::string> ready) {
    if (!ready) {
      m_status = ready.error();
      emit modelsUpdated();
      return;
    }

    // the session is kept for the next conversation
    newSession([this](std::expected<std::string, RpcError> session) {
      if (session) {
        m_status.reset();
        m_available = true;
      } else {
        qWarning() << "Could not start an ACP session with" << m_id << ":" << session.error().message;
        m_status = session.error().code == AUTH_REQUIRED
                       ? signInMessage()
                       : tr("Could not start %1: %2")
                             .arg(qs(displayName()), qs(session.error().message))
                             .toStdString();
        stop(session.error().message);
      }
      emit modelsUpdated();
      updateIdle();
    });
  });
}

void AcpProvider::connectAgent(ReadyHandler handler) {
  if (m_ready) {
    handler({});
    return;
  }
  m_waiting.emplace_back(std::move(handler));
  if (!m_process) launch();
}

void AcpProvider::launch() {
  struct Command {
    QString program;
    QStringList arguments;
  };

  const auto command = [&] -> std::expected<Command, std::string> {
    if (!m_command.empty()) {
      auto parts = QProcess::splitCommand(qs(m_command));
      if (parts.isEmpty()) return std::unexpected(tr("No command is set.").toStdString());
      const auto name = parts.takeFirst();
      const auto program = name.contains(QLatin1Char('/')) ? findCliExecutable(name.toStdString(), name)
                                                           : findCliExecutable({}, name);
      if (!program) return std::unexpected(tr("Could not find %1.").arg(name).toStdString());
      return Command{*program, parts};
    }

    const auto *preset = findPreset(m_type);
    if (!preset) return std::unexpected(tr("No command is set.").toStdString());
    if (auto program = findCliExecutable({}, qs(preset->executable)))
      return Command{*program, qs(preset->arguments).split(QLatin1Char(' '), Qt::SkipEmptyParts)};
    if (!preset->package.empty()) {
      if (auto npx = findCliExecutable({}, QStringLiteral("npx")))
        return Command{*npx, {QStringLiteral("-y"), qs(preset->package)}};
      return std::unexpected(tr("Requires Node.js to run %1, or %2 to be installed.")
                                 .arg(qs(displayName()), qs(preset->executable))
                                 .toStdString());
    }
    return std::unexpected(
        tr("%1 is not installed, or could not be found.").arg(qs(displayName())).toStdString());
  }();

  if (!command) {
    for (const auto &handler : std::exchange(m_waiting, {}))
      handler(std::unexpected(command.error()));
    return;
  }

  m_process = new JsonLineProcess(command->program, command->arguments, this);
  if (const auto *preset = findPreset(m_type); preset && !preset->apiKeyVariable.empty())
    m_process->unsetEnvironment(qs(preset->apiKeyVariable));
  connect(m_process, &JsonLineProcess::lineReceived, this, &AcpProvider::handleLine);
  connect(m_process, &JsonLineProcess::exited, this, [this](const std::string &reason) {
    qWarning() << "ACP agent" << m_id << "exited:" << reason;
    stop(reason);
  });
  m_process->start();

  request("initialize",
          std::format(R"({{"protocolVersion":1,"clientCapabilities":{{"fs":{{"readTextFile":false,)"
                      R"("writeTextFile":false}},"terminal":false}},"clientInfo":{{"name":"vicinae",)"
                      R"("title":"Vicinae","version":"{}"}}}})",
                      VICINAE_GIT_TAG),
          [this](Response response) {
            if (!response) {
              stop(response.error().message);
              return;
            }
            if (auto result = fromJson<acp::InitializeResult>(*response);
                result && result->agentCapabilities) {
              const auto &caps = *result->agentCapabilities;
              m_supportsImages = caps.promptCapabilities && caps.promptCapabilities->image;
              m_supportsClose = caps.sessionCapabilities && present(caps.sessionCapabilities->close);
            }
            m_ready = true;
            for (const auto &handler : std::exchange(m_waiting, {}))
              handler({});
          });
}

void AcpProvider::stop(const std::string &reason) {
  m_ready = false;
  m_idle.stop();

  if (auto *process = std::exchange(m_process, nullptr)) {
    disconnect(process, nullptr, this, nullptr);
    process->shutdown();
  }

  // turns still running end with an error, which needs their sessions
  for (auto &[id, handler] : std::exchange(m_pending, {})) {
    if (handler) handler(std::unexpected(RpcError{.message = reason}));
  }
  for (const auto &handler : std::exchange(m_waiting, {}))
    handler(std::unexpected(reason));

  m_sessions.clear();
}

void AcpProvider::request(std::string_view method, std::string params, ResponseHandler handler) {
  if (!m_process) {
    if (handler) handler(std::unexpected(RpcError{.message = "The agent is not running."}));
    return;
  }

  const auto id = m_nextId++;
  if (handler) m_pending[id] = std::move(handler);
  m_process->write(
      std::format(R"({{"jsonrpc":"2.0","id":{},"method":"{}","params":{}}})", id, method, params));
}

void AcpProvider::respond(const std::string &requestId, std::string_view result) {
  if (!m_process) return;
  m_process->write(std::format(R"({{"jsonrpc":"2.0","id":{},"result":{}}})", requestId, result));
}

void AcpProvider::handleLine(const std::string &line) {
  auto message = fromJson<acp::Incoming>(line);
  if (!message) {
    qWarning() << "Failed to parse ACP message:" << message.error();
    return;
  }

  if (message->method) {
    const std::string_view params = present(message->params) ? std::string_view(message->params->str) : "{}";
    if (message->id) {
      handleRequest(message->id->str, *message->method, params);
    } else if (*message->method == "session/update") {
      handleUpdate(params);
    }
    return;
  }

  if (!message->id) return;

  std::int64_t id = 0;
  const auto &raw = message->id->str;
  if (std::from_chars(raw.data(), raw.data() + raw.size(), id).ec != std::errc{}) return;

  const auto node = m_pending.extract(id);
  if (node.empty() || !node.mapped()) return;

  if (message->error) {
    node.mapped()(std::unexpected(std::move(*message->error)));
  } else {
    node.mapped()(present(message->result) ? message->result->str : std::string("null"));
  }
}

void AcpProvider::handleRequest(const std::string &id, std::string_view method, std::string_view params) {
  if (method != "session/request_permission") {
    if (m_process)
      m_process->write(std::format(
          R"({{"jsonrpc":"2.0","id":{},"error":{{"code":-32601,"message":"Not supported by Vicinae"}}}})",
          id));
    return;
  }

  auto request = fromJson<acp::PermissionRequest>(params);
  const auto *session = request ? findSession(request->sessionId) : nullptr;
  if (!session || !session->stream) {
    respond(id, CANCELLED);
    return;
  }
  session->stream->handlePermission(id, std::move(*request));
}

void AcpProvider::handleUpdate(std::string_view params) {
  auto notification = fromJson<acp::SessionNotification>(params);
  if (!notification) return;
  const auto *session = findSession(notification->sessionId);
  if (session && session->stream) session->stream->handleUpdate(notification->update);
}

AcpProvider::Session *AcpProvider::findSession(std::string_view id) {
  const auto it = std::ranges::find(m_sessions, id, &Session::id);
  return it != m_sessions.end() ? &*it : nullptr;
}

void AcpProvider::newSession(std::function<void(std::expected<std::string, RpcError>)> handler) {
  if (m_sessions.size() >= MAX_SESSIONS) {
    auto idle = m_sessions | std::views::filter([](const Session &session) { return !session.busy; });
    if (const auto oldest = std::ranges::min_element(idle, {}, &Session::lastUsed); oldest != idle.end())
      closeSession(oldest->id);
  }

  request("session/new", toJson(acp::NewSessionParams{.cwd = cliWorkingDirectory().toStdString()}),
          [this, handler = std::move(handler)](Response response) {
            if (!response) {
              handler(std::unexpected(std::move(response.error())));
              return;
            }
            auto result = fromJson<acp::NewSessionResult>(*response);
            if (!result) {
              handler(std::unexpected(RpcError{.message = result.error()}));
              return;
            }
            if (present(result->configOptions)) readModels(result->configOptions->str);
            if (m_models.empty()) readModels("[]");
            m_sessions.emplace_back(Session{
                .id = result->sessionId,
                .fingerprint = QByteArray{},
                .model = m_defaultModel.value_or(std::string{}),
            });
            handler(std::move(result->sessionId));
          });
}

void AcpProvider::readModels(std::string_view json) {
  auto options = fromJson<std::vector<acp::ConfigOption>>(json);
  if (!options) return;

  const auto option = std::ranges::find_if(*options, [](const acp::ConfigOption &option) {
    return option.category == "model" && option.type.value_or("select") == "select" && option.options;
  });

  std::vector<Model> models;
  auto caps = Capabilities{Completion} | Capabilities{Thinking};
  if (m_supportsImages) caps |= Capabilities{Vision};

  if (option == options->end()) {
    m_modelOption.reset();
    m_defaultModel = std::string(DEFAULT_MODEL);
    models.emplace_back(
        Model{.id = std::string(DEFAULT_MODEL), .name = displayName(), .icon = icon(), .caps = caps});
  } else {
    m_modelOption = option->id;
    if (present(option->currentValue))
      m_defaultModel = fromJson<std::string>(option->currentValue->str).value_or("");
    const auto add = [&](const std::string &id, const std::optional<std::string> &name,
                         std::optional<std::string> description) {
      models.emplace_back(Model{
          .id = id,
          .name = name.value_or(id),
          .description = std::move(description),
          .icon = icon(),
          .caps = caps,
      });
    };
    for (auto &entry : *option->options) {
      if (entry.value) add(*entry.value, entry.name, entry.description);
      for (const auto &choice : entry.options.value_or(std::vector<acp::ConfigChoice>{}))
        add(choice.value, choice.name, choice.description);
    }
  }

  const auto same = std::ranges::equal(models, m_models, [](const Model &a, const Model &b) {
    return a.id == b.id && a.name == b.name && a.caps == b.caps;
  });
  if (same) return;
  m_models = std::move(models);
  emit modelsUpdated();
}

void AcpProvider::runTurn(AcpChatStream *stream) {
  m_idle.stop();
  connectAgent([this, stream = QPointer(stream)](std::expected<void, std::string> ready) {
    if (!stream || !stream->active()) return;
    if (!ready) {
      stream->complete(ready.error());
      return;
    }

    if (const auto &id = stream->sessionId()) {
      if (auto *session = findSession(*id)) {
        prompt(*session, stream);
      } else {
        stream->complete(tr("The agent was restarted, send your message again.").toStdString());
      }
      return;
    }

    newSession([this, stream](std::expected<std::string, RpcError> id) {
      if (!stream || !stream->active()) {
        updateIdle();
        return;
      }
      if (!id) {
        stream->complete(id.error().code == AUTH_REQUIRED ? signInMessage() : id.error().message);
        return;
      }
      stream->setSessionId(*id);
      prompt(*findSession(*id), stream);
    });
  });
}

void AcpProvider::prompt(Session &session, AcpChatStream *stream) {
  session.busy = true;
  session.fresh = false;
  session.stream = stream;
  session.lastUsed = std::chrono::steady_clock::now();

  const auto send = [this, id = session.id] {
    auto *session = findSession(id);
    if (!session) return;
    if (!session->stream || !session->stream->active()) {
      session->busy = false;
      session->stream = nullptr;
      updateIdle();
      return;
    }
    request("session/prompt", session->stream->promptJson(),
            [this, id](Response response) { finishTurn(id, std::move(response)); });
  };

  const auto &model = stream->modelId();
  if (!m_modelOption || model == session.model) {
    send();
    return;
  }

  request("session/set_config_option", toJson(acp::SetConfigOptionParams{session.id, *m_modelOption, model}),
          [this, id = session.id, model, send](Response response) {
            if (response) {
              if (auto *session = findSession(id)) session->model = model;
              send();
            } else {
              finishTurn(id, std::move(response));
            }
          });
}

void AcpProvider::finishTurn(const std::string &sessionId, Response response) {
  auto *session = findSession(sessionId);
  if (!session) return;

  const auto stream = std::exchange(session->stream, nullptr);
  const bool active = stream && stream->active();
  session->busy = false;
  session->lastUsed = std::chrono::steady_clock::now();

  std::optional<std::string> error;
  if (!response) {
    error = response.error().code == AUTH_REQUIRED ? signInMessage() : response.error().message;
  } else if (auto result = fromJson<acp::PromptResult>(*response);
             result && result->stopReason == "refusal") {
    error = tr("The agent declined to respond.").toStdString();
  }

  // an interrupted turn leaves the agent with a reply that isn't ours
  session->fingerprint = active && !error ? std::optional(stream->fingerprint()) : std::nullopt;
  if (stream && stream->oneShot()) closeSession(sessionId);
  updateIdle();
  if (active) stream->complete(std::move(error));
}

void AcpProvider::cancelTurn(const std::string &sessionId) {
  auto *session = findSession(sessionId);
  if (!session || !m_process) return;
  session->stream = nullptr;
  m_process->write(std::format(R"({{"jsonrpc":"2.0","method":"session/cancel","params":{}}})",
                               toJson(acp::SessionRef{sessionId})));
}

void AcpProvider::closeSession(const std::string &sessionId) {
  if (m_supportsClose) request("session/close", toJson(acp::SessionRef{sessionId}));
  std::erase_if(m_sessions, [&](const Session &session) { return session.id == sessionId; });
}

void AcpProvider::updateIdle() {
  const bool busy = !m_waiting.empty() || std::ranges::any_of(m_sessions, &Session::busy);
  if (busy || !m_process) {
    m_idle.stop();
  } else {
    m_idle.start();
  }
}

ModelList AcpProvider::listModels(const ListModelFilters &filters) const {
  if (!m_available) return {};
  return m_models | std::views::filter([&](const Model &model) {
           return !filters.caps || (model.caps & *filters.caps) == *filters.caps;
         }) |
         std::ranges::to<ModelList>();
}

std::optional<Model> AcpProvider::findBestModel(Capabilities caps, Preference preference) const {
  const auto models = listModels({.caps = caps});
  if (models.empty()) return std::nullopt;

  if (preference == Preference::Fast) {
    const auto fast = std::ranges::find_if(models, [](const Model &model) {
      const auto id = QString::fromStdString(model.id).toLower();
      return std::ranges::any_of(FAST_MARKERS,
                                 [&](std::string_view marker) { return id.contains(qs(marker)); });
    });
    if (fast != models.end()) return *fast;
  }

  if (m_defaultModel) {
    if (const auto it = std::ranges::find(models, *m_defaultModel, &Model::id); it != models.end())
      return *it;
  }
  return models.front();
}

std::shared_ptr<AbstractChatCompletionStream>
AcpProvider::createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) {
  if (!m_available) return nullptr;
  const auto model = std::ranges::find(m_models, modelId, &Model::id);
  if (model == m_models.end()) return nullptr;

  std::string instructions;
  ChatHistory messages;
  messages.reserve(payload.messages.size());
  for (const auto &message : payload.messages) {
    if (message.role == ChatRole::System) {
      if (!instructions.empty()) instructions += "\n\n";
      instructions += message.text();
    } else {
      messages.emplace_back(message);
    }
  }
  if (messages.empty() || messages.back().role != ChatRole::User) return nullptr;

  auto message = std::move(messages.back());
  messages.pop_back();
  auto fingerprint = historyFingerprint(messages);

  const Session *session = nullptr;
  if (!payload.oneShot) {
    auto candidates = m_sessions | std::views::filter([&](const Session &session) {
                        return !session.busy && session.fingerprint == fingerprint;
                      });
    // switching models is cheap, but not switching is cheaper
    const auto preferred = std::ranges::find(candidates, model->id, &Session::model);
    if (preferred != candidates.end()) {
      session = &*preferred;
    } else if (!candidates.empty()) {
      session = &candidates.front();
    }
  }

  auto prompt = makePrompt(!session || session->fresh ? instructions : std::string{},
                           session ? ChatHistory{} : compactHistory(messages), message, m_supportsImages);

  return std::make_shared<AcpChatStream>(this, session ? std::optional(session->id) : std::nullopt, model->id,
                                         std::move(prompt), std::move(fingerprint), std::move(message),
                                         *model, payload.oneShot);
}

QFuture<TranscriptionResult> AcpProvider::transcribe(Audio::Recording, const TranscriptionOptions &) {
  return QtFuture::makeReadyValueFuture<TranscriptionResult>(
      std::unexpected("Agents do not support transcription"));
}

} // namespace AI
