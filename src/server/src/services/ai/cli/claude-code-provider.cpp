#include "claude-code-provider.hpp"
#include <algorithm>
#include <format>
#include <ranges>
#include <span>
#include <glaze/json.hpp>
#include <QCryptographicHash>
#include <QProcess>
#include "cli-process.hpp"
#include "services/ai/ai-provider-types.hpp"
#include "services/builtin-icon/builtin-icon.hpp"

namespace AI {

namespace {

namespace claude {

struct ImageSource {
  std::string type = "base64";
  std::string media_type;
  std::string data;
};

struct ContentBlock {
  std::string type;
  std::optional<std::string> text;
  std::optional<ImageSource> source;
};

struct UserMessage {
  std::string role = "user";
  std::vector<ContentBlock> content;
};

struct Input {
  std::string type = "user";
  UserMessage message;
};

struct Event {
  struct Stream {
    struct Delta {
      std::optional<std::string> type;
      std::optional<std::string> text;
    };
    struct Block {
      std::optional<std::string> type;
    };

    std::string type;
    std::optional<Delta> delta;
    std::optional<Block> content_block;
  };

  std::string type;
  std::optional<std::string> subtype;
  std::optional<Stream> event;
  std::optional<bool> is_error;
  std::optional<std::string> result;
  std::optional<std::vector<std::string>> errors;
};

struct AuthStatus {
  bool loggedIn = false;
};

} // namespace claude

constexpr glz::opts LENIENT{.error_on_unknown_keys = false};

struct ClaudeModel {
  std::string_view id;
  std::string_view name;
};

constexpr auto MODELS = std::to_array<ClaudeModel>({
    {"sonnet", "Claude Sonnet"},
    {"opus", "Claude Opus"},
    {"haiku", "Claude Haiku"},
});

constexpr Capabilities MODEL_CAPS = Capability::Completion | Capability::Vision | Capability::Thinking;

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

QByteArray chainFingerprint(QByteArrayView previous, const ChatMessage &message) {
  QCryptographicHash hash(QCryptographicHash::Sha256);
  const auto add = [&](char tag, std::string_view data) {
    hash.addData(QByteArrayView(&tag, 1));
    hash.addData(QByteArrayView(data.data(), static_cast<qsizetype>(data.size())));
    hash.addData(QByteArrayView("\0", 1));
  };

  hash.addData(previous);
  add('m', roleName(message.role));
  for (const auto &part : message.parts) {
    if (const auto *text = std::get_if<TextPart>(&part)) {
      add('t', text->text);
    } else if (const auto *image = std::get_if<ImagePart>(&part)) {
      add('i', image->mimeType);
      hash.addData(image->base64);
    } else if (const auto *call = std::get_if<ToolCallPart>(&part)) {
      add('c', call->name);
      add('a', call->arguments);
    } else if (const auto *result = std::get_if<ToolResultPart>(&part)) {
      add('r', result->content);
    }
  }

  return hash.result();
}

void appendBlocks(std::vector<claude::ContentBlock> &blocks, const ChatMessage &message) {
  for (const auto &part : message.parts) {
    if (const auto *text = std::get_if<TextPart>(&part)) {
      blocks.emplace_back(claude::ContentBlock{.type = "text", .text = text->text});
    } else if (const auto *image = std::get_if<ImagePart>(&part)) {
      blocks.emplace_back(claude::ContentBlock{
          .type = "image",
          .source = claude::ImageSource{.media_type = image->mimeType, .data = image->base64.toStdString()},
      });
    } else if (const auto *call = std::get_if<ToolCallPart>(&part)) {
      blocks.emplace_back(claude::ContentBlock{
          .type = "text", .text = std::format("(called tool {} with {})", call->name, call->arguments)});
    } else if (const auto *result = std::get_if<ToolResultPart>(&part)) {
      blocks.emplace_back(
          claude::ContentBlock{.type = "text", .text = std::format("(tool result: {})", result->content)});
    }
  }
}

/**
 * The CLI cannot be handed earlier assistant turns, so a conversation it has not seen is replayed as a
 * transcript inside the first message.
 */
std::string makeInput(std::span<const ChatMessage> history, const ChatMessage &message) {
  claude::Input input;
  auto &blocks = input.message.content;

  if (!history.empty()) {
    blocks.emplace_back(claude::ContentBlock{
        .type = "text",
        .text = "This conversation started elsewhere. Here is what was said so far:\n\n<conversation>"});

    for (const auto &entry : history) {
      const auto tag = entry.role == ChatRole::Assistant ? "assistant" : "user";
      blocks.emplace_back(claude::ContentBlock{.type = "text", .text = std::format("<{}>", tag)});
      appendBlocks(blocks, entry);
      blocks.emplace_back(claude::ContentBlock{.type = "text", .text = std::format("</{}>", tag)});
    }

    blocks.emplace_back(claude::ContentBlock{
        .type = "text", .text = "</conversation>\n\nContinue it by replying to this message:"});
  }

  appendBlocks(blocks, message);

  std::string out;
  if (glz::write_json(input, out)) return {};
  return out;
}

class ClaudeChatStream : public AbstractChatCompletionStream {
public:
  ClaudeChatStream(ClaudeSession *session, std::string line, QByteArray fingerprint, Model model)
      : m_session(session), m_line(std::move(line)), m_fingerprint(std::move(fingerprint)) {
    setModel(std::move(model));
  }

  ~ClaudeChatStream() override { abort(); }

  bool start() override {
    if (!m_session || m_line.empty()) return false;

    connect(m_session, &ClaudeSession::activityChanged, this, &ClaudeChatStream::activityChanged);
    connect(m_session, &ClaudeSession::textReceived, this, &ClaudeChatStream::dataAdded);
    connect(m_session, &ClaudeSession::turnSucceeded, this, [this] { complete({}); });
    connect(m_session, &ClaudeSession::turnFailed, this,
            [this](const std::string &reason) { complete(reason); });
    connect(m_session, &ClaudeSession::closed, this,
            [this] { complete("Claude Code stopped before finishing the response."); });

    m_active = true;
    m_session->send(m_line, std::move(m_fingerprint));
    return true;
  }

  // The CLI keeps the interrupted reply in its history, which would no longer match ours.
  bool abort() override {
    if (!std::exchange(m_active, false)) return true;
    if (m_session) {
      disconnect(m_session, nullptr, this, nullptr);
      m_session->close();
    }
    return true;
  }

private:
  void complete(std::optional<std::string> error) {
    if (!std::exchange(m_active, false)) return;
    disconnect(m_session, nullptr, this, nullptr);
    if (error) {
      emit errorOccurred(*error);
    } else {
      emit finished();
    }
  }

  QPointer<ClaudeSession> m_session;
  std::string m_line;
  QByteArray m_fingerprint;
  bool m_active = false;
};

} // namespace

ClaudeSession::ClaudeSession(const QString &executable, std::string model, std::string effort,
                             const std::string &systemPrompt, QObject *parent)
    : QObject(parent), m_model(std::move(model)), m_effort(std::move(effort)) {
  QStringList args{
      "-p",
      "--input-format",
      "stream-json",
      "--output-format",
      "stream-json",
      "--verbose",
      "--include-partial-messages",
      "--model",
      QString::fromStdString(m_model),
      "--effort",
      QString::fromStdString(m_effort),
      "--tools",
      "",
      "--strict-mcp-config",
      "--setting-sources",
      "",
      "--no-session-persistence",
  };
  if (!systemPrompt.empty()) args << "--system-prompt" << QString::fromStdString(systemPrompt);

  m_process = new JsonLineProcess(executable, args, this);
  connect(m_process, &JsonLineProcess::lineReceived, this, &ClaudeSession::handleLine);
  connect(m_process, &JsonLineProcess::exited, this, [this](const std::string &reason) {
    qWarning() << "Claude Code session exited:" << reason;
    if (m_busy) emit turnFailed(reason);
    close();
  });

  m_idle.setSingleShot(true);
  m_idle.setInterval(IDLE_TIMEOUT);
  connect(&m_idle, &QTimer::timeout, this, &ClaudeSession::close);

  m_process->start();
}

ClaudeSession::~ClaudeSession() {
  if (m_process) m_process->shutdown();
}

void ClaudeSession::send(const std::string &line, QByteArray fingerprint) {
  m_idle.stop();
  m_busy = true;
  m_fingerprint.reset();
  m_pendingFingerprint = std::move(fingerprint);
  m_text.clear();
  m_lastUsed = std::chrono::steady_clock::now();
  m_process->write(line);
}

void ClaudeSession::close() {
  if (std::exchange(m_closed, true)) return;
  m_busy = false;
  m_fingerprint.reset();
  m_idle.stop();
  if (auto *process = std::exchange(m_process, nullptr)) {
    disconnect(process, nullptr, this, nullptr);
    process->shutdown();
  }
  emit closed();
}

void ClaudeSession::endTurn() {
  m_busy = false;
  m_lastUsed = std::chrono::steady_clock::now();
  m_idle.start();
}

void ClaudeSession::handleLine(const std::string &line) {
  if (!m_busy) return;

  claude::Event event;
  if (const auto error = glz::read<LENIENT>(event, line)) {
    qWarning() << "Failed to parse Claude Code event" << glz::format_error(error, line);
    return;
  }

  if (event.type == "stream_event" && event.event) {
    const auto &stream = *event.event;
    if (stream.type == "content_block_start" && stream.content_block &&
        stream.content_block->type == "thinking") {
      emit activityChanged(ResponseActivity::Thinking);
    } else if (stream.type == "content_block_delta" && stream.delta) {
      if (stream.delta->type == "thinking_delta") emit activityChanged(ResponseActivity::Thinking);
      if (stream.delta->type == "text_delta" && stream.delta->text && !stream.delta->text->empty()) {
        m_text += *stream.delta->text;
        emit textReceived(*stream.delta->text);
      }
    }
    return;
  }

  if (event.type != "result") return;

  if (event.is_error.value_or(false) || event.subtype != "success") {
    std::string reason = event.result.value_or("");
    if (reason.empty() && event.errors && !event.errors->empty()) reason = event.errors->front();
    if (reason.empty()) reason = "Claude Code could not respond.";
    endTurn();
    emit turnFailed(reason);
    close();
    return;
  }

  m_fingerprint = chainFingerprint(m_pendingFingerprint, ChatMessage::fromText(ChatRole::Assistant, m_text));
  endTurn();
  emit turnSucceeded();
}

ClaudeCodeProvider::ClaudeCodeProvider(std::string id) : m_id(std::move(id)) {}

ClaudeCodeProvider::~ClaudeCodeProvider() {
  if (m_detection) m_detection->kill();
  closeSessions();
}

std::string ClaudeCodeProvider::displayName() const { return std::string(findProviderType(TYPE)->label); }

std::optional<ImageUrl> ClaudeCodeProvider::icon() const { return ImageUrl{BuiltinIcon::ClaudeCode}; }

std::string_view ClaudeCodeProvider::description() const { return findProviderType(TYPE)->description; }

void ClaudeCodeProvider::configure(const PreferenceValues &values) {
  auto fields = readPreferences<ClaudeCodeFields>(values);
  if (fields.executable == m_executableSetting && m_started) return;

  m_executableSetting = std::move(fields.executable);
  closeSessions();
  if (m_started) detect();
}

void ClaudeCodeProvider::start() {
  m_started = true;
  detect();
}

void ClaudeCodeProvider::detect() {
  if (m_detection) {
    disconnect(m_detection, nullptr, this, nullptr);
    m_detection->kill();
    m_detection->deleteLater();
  }

  m_available = false;
  m_executable = findCliExecutable(m_executableSetting, QStringLiteral("claude"));

  if (!m_executable) {
    m_status =
        m_executableSetting.empty()
            ? tr("Claude Code is not installed, or could not be found.").toStdString()
            : tr("No executable at %1.").arg(QString::fromStdString(m_executableSetting)).toStdString();
    emit modelsUpdated();
    return;
  }

  m_status = tr("Checking Claude Code…").toStdString();
  emit modelsUpdated();

  m_detection = new QProcess(this);
  m_detection->setWorkingDirectory(cliWorkingDirectory());
  connect(m_detection, &QProcess::finished, this, [this](int exitCode) { handleDetection(exitCode); });
  connect(m_detection, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
    if (error == QProcess::FailedToStart) handleDetection(-1);
  });
  m_detection->start(*m_executable, {"auth", "status", "--json"});
}

void ClaudeCodeProvider::handleDetection(int exitCode) {
  QProcess *process = m_detection;
  if (!process) return;
  m_detection = nullptr;
  process->deleteLater();

  const auto output = process->readAllStandardOutput().toStdString();
  claude::AuthStatus status;
  const auto error = glz::read<LENIENT>(status, output);

  if (exitCode == -1 || (error && exitCode != 0)) {
    qWarning() << "Failed to check Claude Code authentication:" << process->readAllStandardError();
    m_status = tr("Could not run Claude Code.").toStdString();
  } else if (!status.loggedIn) {
    m_status = tr("Not signed in. Run \"claude auth login\" in a terminal.").toStdString();
  } else {
    m_status.reset();
    m_available = true;
  }

  emit modelsUpdated();
}

void ClaudeCodeProvider::closeSessions() {
  for (auto *session : std::exchange(m_sessions, {})) {
    disconnect(session, nullptr, this, nullptr);
    session->close();
    session->deleteLater();
  }
}

ModelList ClaudeCodeProvider::listModels(const ListModelFilters &filters) const {
  if (!m_available || (filters.caps && !(*filters.caps & MODEL_CAPS))) return {};

  return MODELS | std::views::transform([](const ClaudeModel &model) {
           return Model{.id = std::string(model.id),
                        .name = std::string(model.name),
                        .icon = ImageUrl{BuiltinIcon::Claude},
                        .caps = MODEL_CAPS};
         }) |
         std::ranges::to<ModelList>();
}

std::optional<Model> ClaudeCodeProvider::findBestModel(Capabilities caps, Preference preference) const {
  const auto models = listModels({.caps = caps});
  const auto id = preference == Preference::Fast        ? "haiku"
                  : preference == Preference::Reasoning ? "opus"
                                                        : "sonnet";
  if (auto it = std::ranges::find(models, id, &Model::id); it != models.end()) return *it;
  return std::nullopt;
}

ClaudeSession *ClaudeCodeProvider::findSession(std::string_view model, std::string_view effort,
                                               const QByteArray &fingerprint) const {
  const auto it = std::ranges::find_if(m_sessions, [&](const ClaudeSession *session) {
    return !session->busy() && session->model() == model && session->effort() == effort &&
           session->fingerprint() == fingerprint;
  });
  return it != m_sessions.end() ? *it : nullptr;
}

ClaudeSession *ClaudeCodeProvider::createSession(std::string model, std::string effort,
                                                 const std::string &systemPrompt) {
  if (m_sessions.size() >= MAX_SESSIONS) {
    auto idle = m_sessions | std::views::filter([](const ClaudeSession *s) { return !s->busy(); });
    if (auto oldest = std::ranges::min_element(idle, {}, &ClaudeSession::lastUsed); oldest != idle.end()) {
      (*oldest)->close();
    }
  }

  auto *session = new ClaudeSession(*m_executable, std::move(model), std::move(effort), systemPrompt, this);
  connect(session, &ClaudeSession::closed, this, [this, session] {
    std::erase(m_sessions, session);
    session->deleteLater();
  });
  m_sessions.emplace_back(session);
  return session;
}

std::shared_ptr<AbstractChatCompletionStream>
ClaudeCodeProvider::createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) {
  if (!m_available || !m_executable) return nullptr;

  const auto model = std::ranges::find(MODELS, modelId, &ClaudeModel::id);
  if (model == MODELS.end()) return nullptr;

  std::string systemPrompt;
  std::vector<ChatMessage> messages;
  messages.reserve(payload.messages.size());
  for (const auto &message : payload.messages) {
    if (message.role == ChatRole::System || message.role == ChatRole::Developer) {
      if (!systemPrompt.empty()) systemPrompt += "\n\n";
      systemPrompt += message.text();
    } else {
      messages.emplace_back(message);
    }
  }

  if (messages.empty() || messages.back().role != ChatRole::User) return nullptr;

  const auto history = std::span(messages).first(messages.size() - 1);
  QByteArray fingerprint;
  for (const auto &message : history)
    fingerprint = chainFingerprint(fingerprint, message);

  const auto effort = effortFor(payload.thinking);
  auto *session = findSession(modelId, effort, fingerprint);
  std::string line;

  if (session) {
    line = makeInput({}, messages.back());
  } else {
    session = createSession(std::string(modelId), effort, systemPrompt);
    line = makeInput(history, messages.back());
  }

  fingerprint = chainFingerprint(fingerprint, messages.back());
  return std::make_shared<ClaudeChatStream>(
      session, std::move(line), std::move(fingerprint),
      Model{.id = std::string(model->id), .name = std::string(model->name), .caps = MODEL_CAPS});
}

QFuture<TranscriptionResult> ClaudeCodeProvider::transcribe(Audio::Recording, const TranscriptionOptions &) {
  return QtFuture::makeReadyValueFuture<TranscriptionResult>(
      std::unexpected("Claude Code does not support transcription"));
}

} // namespace AI
