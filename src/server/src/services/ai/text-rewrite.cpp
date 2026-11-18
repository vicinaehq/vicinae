#include "text-rewrite.hpp"
#include <algorithm>
#include <chrono>
#include <format>
#include "services/ai/ai-service.hpp"

namespace {

constexpr auto REWRITE_TIMEOUT = std::chrono::seconds(30);
constexpr qsizetype MIN_OUTPUT_TOKENS = 1024;

QString cleanOutput(const std::string &output, const std::optional<std::string> &tag) {
  auto text = QString::fromStdString(output).trimmed();
  if (!tag) return text;
  const auto opening = QString::fromStdString(std::format("<{}>", *tag));
  const auto closing = QString::fromStdString(std::format("</{}>", *tag));
  if (text.startsWith(opening) && text.endsWith(closing)) {
    text = text.sliced(opening.size(), text.size() - opening.size() - closing.size()).trimmed();
  }
  return text;
}

} // namespace

TextRewrite::TextRewrite(AI::Service &ai, QObject *parent) : QObject(parent), m_ai(ai) {
  m_timeout.setSingleShot(true);
  m_timeout.setInterval(REWRITE_TIMEOUT);
  connect(&m_timeout, &QTimer::timeout, this, [this] { fail(tr("The model took too long to respond.")); });
}

TextRewrite::~TextRewrite() { abort(); }

void TextRewrite::start(const Request &request) {
  abort();
  m_running = true;
  m_text.clear();
  m_echoTag = request.echoTag;

  const auto ref = AI::ModelRef::fromString(request.model);
  const auto models = m_ai.listModels(AI::Capability::Completion);
  const bool available = ref && std::ranges::any_of(models, [&](const AI::ProviderModel &model) {
                           return model.ref.provider == ref->provider && model.ref.id == ref->id;
                         });

  if (!available) {
    QTimer::singleShot(0, this, [this] {
      if (m_running) fail(tr("The model is not available."));
    });
    return;
  }

  const auto inputSize = static_cast<qsizetype>(request.prompt.size());
  AI::ChatCompletionPayload payload{.thinking = AI::ThinkingMode::None,
                                    .maxOutputTokens = std::max(MIN_OUTPUT_TOKENS, 2 * inputSize),
                                    .oneShot = true};
  payload.messages.reserve(2);
  payload.messages.emplace_back(AI::ChatMessage::fromText(AI::ChatRole::System, request.systemPrompt));
  payload.messages.emplace_back(AI::ChatMessage::fromText(AI::ChatRole::User, request.prompt));

  m_stream = m_ai.createChatCompletion(*ref, payload);
  if (!m_stream) {
    QTimer::singleShot(0, this, [this] {
      if (m_running) fail(tr("Could not start the rewrite."));
    });
    return;
  }

  connect(m_stream.get(), &AI::AbstractChatCompletionStream::dataAdded, this,
          [this](const std::string &text) { m_text += text; });
  connect(m_stream.get(), &AI::AbstractChatCompletionStream::errorOccurred, this,
          [this](const std::string &reason) { fail(QString::fromStdString(reason)); });
  connect(m_stream.get(), &AI::AbstractChatCompletionStream::finished, this, &TextRewrite::complete);

  m_timeout.start();
  if (const auto stream = m_stream; !stream->start() && m_running) fail(tr("Could not start the rewrite."));
}

void TextRewrite::abort() {
  if (!m_running) return;
  m_running = false;
  if (m_stream) {
    disconnect(m_stream.get(), nullptr, this, nullptr);
    m_stream->abort();
  }
  release();
}

void TextRewrite::complete() {
  if (!m_running) return;
  const auto text = cleanOutput(m_text, m_echoTag);
  if (text.isEmpty()) {
    fail(tr("The model returned an empty response."));
    return;
  }
  m_running = false;
  release();
  emit finished(text);
}

void TextRewrite::fail(const QString &reason) {
  if (!m_running) return;
  m_running = false;
  if (m_stream) {
    disconnect(m_stream.get(), nullptr, this, nullptr);
    m_stream->abort();
  }
  release();
  emit failed(reason);
}

void TextRewrite::release() {
  m_timeout.stop();
  if (!m_stream) return;
  disconnect(m_stream.get(), nullptr, this, nullptr);
  // this can run from one of the stream's own signals, it can't be destroyed before returning
  QTimer::singleShot(0, this, [stream = std::move(m_stream)] {});
  m_stream.reset();
}
